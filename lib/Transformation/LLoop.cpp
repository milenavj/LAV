//===-- LLoop.cpp ---------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "floop-unroll"

#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/LLVMContext.h"

//#include "llvm/InitializePasses.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/Dominators.h"
#include "llvm/Analysis/ConstantFolding.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/DominanceFrontier.h"
#include "llvm/Analysis/InstructionSimplify.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/LoopIterator.h"

#include "llvm/Support/Compiler.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils/UnrollLoop.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/Local.h"

//FIXME izbaciti sprintf pa onda i ovo stdio
#include "stdio.h"

#include <climits>
#include <iostream>
#include <map>
#include <set>
#include <vector>

using namespace llvm;

namespace lav {
//blok --- broj petlje, broj razmotavanja u okviru petlje
//npr bb1 --- (4,7) (3,5)  => blok bb1 pripada trecoj petlji, u okviru petog
//razmotavanja je cetvrta petlja i njeno sedmo razmotavanje
std::map<BasicBlock *, std::vector<std::pair<unsigned, unsigned> > >
    FLoopBlocks;
}

namespace {
llvm::cl::opt<unsigned> UnrollLoopBegin(
    "loop-unroll-begin",
    llvm::cl::desc("LAV --- Loop unrolling - beginning (default=2)"),
    llvm::cl::init(2));

llvm::cl::opt<unsigned>
    UnrollLoopEnd("loop-unroll-end",
                  llvm::cl::desc("LAV --- Loop unrolling - end (default=1)"),
                  llvm::cl::init(1));

}

namespace {

class FLoopUnroll : public LoopPass {
public:
  static char ID; // Pass ID, replacement for typeid
  FLoopUnroll() : LoopPass(ID) {
    //   initializeFLoopUnrollPass(*PassRegistry::getPassRegistry());
  }

  /// A magic value for use with the Threshold parameter to indicate
  /// that the loop unroll should be performed regardless of how much
  /// code expansion would result.

  virtual bool runOnLoop(Loop *L, LPPassManager &LPM);
  //    virtual bool doInitialization(Loop *, LPPassManager &LPM);
  //    virtual bool doFinalization();

  /// This transformation requires natural loop information & requires that
  /// loop preheaders be inserted into the CFG...
  ///
  virtual void getAnalysisUsage(AnalysisUsage &AU) const {
    AU.addRequiredID(LoopSimplifyID);
    AU.addRequiredID(LCSSAID);
    AU.addRequired<LoopInfo>();

    AU.addPreservedID(LCSSAID);
    AU.addPreserved<LoopInfo>();

    // FIXME: Loop unroll requires LCSSA. And LCSSA requires dom info.
    // If loop unroll does not preserve dom info then LCSSA pass on next
    // loop will receive invalid dom info.
    // For now, recreate dom info, if loop is unrolled.
    AU.addPreserved<DominatorTree>();
    AU.addPreserved<DominanceFrontier>();

    //ako se ovo doda onda postoje neke zavisnosti zbog kojih puca sve
    //t       AU.addRequired<ScalarEvolution>();
    AU.addPreserved<ScalarEvolution>();

    //FIXME ovo je sumnjivo
    AU.addPreserved<AliasAnalysis>();
  }
};
}

char FLoopUnroll::ID = 0;
static RegisterPass<FLoopUnroll> X("lloop-unroll", "LUnroll loops");

namespace lav {
Pass *createFLoopUnrollPass() { return new FLoopUnroll(); }
}

unsigned loop_no = 0;

/// RemapInstruction - Convert the instruction operands from referencing the
/// current values into those specified by VMap.
static inline void RemapInstruction1(Instruction *I, ValueToValueMapTy &VMap) {
  for (unsigned op = 0, E = I->getNumOperands(); op != E; ++op) {
    Value *Op = I->getOperand(op);
    ValueToValueMapTy::iterator It = VMap.find(Op);
    if (It != VMap.end())
      I->setOperand(op, It->second);
  }
  if (PHINode *PN = dyn_cast<PHINode>(I)) {
    for (unsigned i = 0, e = PN->getNumIncomingValues(); i != e; ++i) {
      ValueToValueMapTy::iterator It = VMap.find(PN->getIncomingBlock(i));
      if (It != VMap.end())
        PN->setIncomingBlock(i, cast<BasicBlock>(It->second));
    }
  }

}

/// FoldBlockIntoPredecessor - Folds a basic block into its predecessor if it
/// only has one predecessor, and that predecessor only has one successor.
/// The LoopInfo Analysis that is passed will be kept consistent.
/// Returns the new combined block.
static BasicBlock *FoldBlockIntoPredecessor(BasicBlock *BB, LoopInfo *LI,
                                            LPPassManager *LPM) {
  // Merge basic blocks into their predecessor if there is only one distinct
  // pred, and if there is only one distinct successor of the predecessor, and
  // if there are no PHI nodes.
  BasicBlock *OnlyPred = BB->getSinglePredecessor();
  if (!OnlyPred)
    return 0;

  if (OnlyPred->getTerminator()->getNumSuccessors() != 1)
    return 0;

  DEBUG(dbgs() << "Merging: " << *BB << "into: " << *OnlyPred);

  // Resolve any PHI nodes at the start of the block.  They are all
  // guaranteed to have exactly one entry if they exist, unless there are
  // multiple duplicate (but guaranteed to be equal) entries for the
  // incoming edges.  This occurs when there are multiple edges from
  // OnlyPred to OnlySucc.
  FoldSingleEntryPHINodes(BB);

  // Delete the unconditional branch from the predecessor...
  OnlyPred->getInstList().pop_back();

  // Make all PHI nodes that referred to BB now refer to Pred as their
  // source...
  BB->replaceAllUsesWith(OnlyPred);

  // Move all definitions in the successor to the predecessor...
  OnlyPred->getInstList().splice(OnlyPred->end(), BB->getInstList());

  std::string OldName = BB->getName();

  // ScalarEvolution holds references to loop exit blocks.
  if (LPM) {
    if (ScalarEvolution *SE = LPM->getAnalysisIfAvailable<ScalarEvolution>()) {
      if (Loop *L = LI->getLoopFor(BB))
        SE->forgetLoop(L);
    }
  }
  // Erase basic block from the function...
  LI->removeBlock(BB);
  BB->eraseFromParent();

  // Inherit predecessor's name if it exists...
  if (!OldName.empty() && !OnlyPred->hasName())
    OnlyPred->setName(OldName);

  return OnlyPred;
}

void GetIncValues(Loop *L, std::set<Value *> &IncValues) {
  //FIXME ovo povecavanje nije sve jedno u kojem je bloku tj da li se uvek
  //izvrsi
  for (std::vector<BasicBlock *>::const_iterator BB = L->getBlocks().begin(),
                                                 E = L->getBlocks().end();
       BB != E; ++BB) {
    BasicBlock::iterator bbit = (*BB)->begin(), bbie = (*BB)->end();
    for (; bbit != bbie; ++bbit) {
      Instruction *i = bbit;
      if (i->getOpcode() == Instruction::Load) {
        BasicBlock::iterator bbit1 = bbit;
        if ((++bbit1) == bbie)
          continue;
        Instruction *i1 = bbit1;
        if (i1->getOpcode() != Instruction::Add)
          continue;
        const llvm::ConstantInt *ci =
            llvm::dyn_cast<llvm::ConstantInt>(i1->getOperand(1));
        if (!ci)
          continue;
        if (!((ci->getValue().toString(10, true) == "1") ||
              (ci->getValue().toString(10, true) == "2")))
          continue;
        //FIXME proveriti da li je add od prethodnog load=
        //if(i1->getOperand(0) != i->)
        BasicBlock::iterator bbit2 = bbit1;
        if ((++bbit2) == bbie)
          continue;
        Instruction *i2 = bbit2;

        if (i2->getOpcode() != Instruction::Store)
          continue;
        if (i2->getOperand(1) != i->getOperand(0))
          continue;
        //FIXME treba proveriti da se ova vrednost ne menja jos nekako
        IncValues.insert(i->getOperand(0));
      }
    }
  }
}

void GetDecValues(Loop *L, std::set<Value *> &DecValues) {
  //FIXME ovo povecavanje nije sve jedno u kome je bloku tj da li se uvek izvrsi
  for (std::vector<BasicBlock *>::const_iterator BB = L->getBlocks().begin(),
                                                 E = L->getBlocks().end();
       BB != E; ++BB) {
    BasicBlock::iterator bbit = (*BB)->begin(), bbie = (*BB)->end();
    for (; bbit != bbie; ++bbit) {
      Instruction *i = bbit;
      if (i->getOpcode() == Instruction::Load) {
        BasicBlock::iterator bbit1 = bbit;
        if ((++bbit1) == bbie)
          continue;
        Instruction *i1 = bbit1;

        if ((i1->getOpcode() == Instruction::Sub) ||
            (i1->getOpcode() == Instruction::Add)) {
          const llvm::ConstantInt *ci =
              llvm::dyn_cast<llvm::ConstantInt>(i1->getOperand(1));
          if (!ci)
            continue;

          if ((i1->getOpcode() == Instruction::Sub) &&
              !((ci->getValue().toString(10, true) == "1") ||
                (ci->getValue().toString(10, true) == "2")))
            continue;
          if ((i1->getOpcode() == Instruction::Add) &&
              !((ci->getValue().toString(10, true) == "-1") ||
                (ci->getValue().toString(10, true) == "-2")))
            continue;
          //FIXME proveriti da li je add od prethodnog load=
          //if(i1->getOperand(0) != i->)
          BasicBlock::iterator bbit2 = bbit1;
          if ((++bbit2) == bbie)
            continue;
          Instruction *i2 = bbit2;

          if (i2->getOpcode() != Instruction::Store)
            continue;
          if (i2->getOperand(1) != i->getOperand(0))
            continue;
          //FIXME treba proveriti da se ova vrednost ne menja jos nekako
          DecValues.insert(i->getOperand(0));
        }
      }
    }
  }
}

//-----------------------------------------
//KOJE SE SVE VREDNOSTI MENJAJU U PETLJI
//FIXME ovde bi trebalo ukljuciti AA
void GetChangedValues(Loop *L, std::set<Value *> &ChangedValues) {
  //std::vector<B, aasicBlock*> LoopBlocks = L->getBlocks();

  for (std::vector<BasicBlock *>::const_iterator BB = L->getBlocks().begin(),
                                                 E = L->getBlocks().end();
       BB != E; ++BB) {
    BasicBlock::iterator bbit = (*BB)->begin(), bbie = (*BB)->end();
    for (; bbit != bbie; ++bbit) {
      Instruction *i = bbit;
      //FIXME Promenljiva moze da se izmeni i preko poziva funkcije pa bi i to
      //trebalo uzeti u obzir
      if (i->getOpcode() == Instruction::Store) {
        //FIXME ovo nece raditi za getelementptr
        ChangedValues.insert(i->getOperand(1));
      }
    }
  }

}

void RemapBlockInstructions(BasicBlock *BB, ValueToValueMapTy &LastValueMap) {
  for (BasicBlock::iterator I = BB->begin(), E = BB->end(); I != E; ++I)
    RemapInstruction1(I, LastValueMap);
}

void RemapAllInstructions(std::vector<BasicBlock *> &NewBlocks,
                          ValueToValueMapTy &LastValueMap) {
  // Remap all instructions in the most recent iteration
  for (unsigned i = 0; i < NewBlocks.size(); ++i)
    RemapBlockInstructions(NewBlocks[i], LastValueMap);
}

std::set<BasicBlock *> obrisati;

void DodajObelezje(BasicBlock *BB, BasicBlock *New, unsigned brRazmotavanja) {
  std::vector<std::pair<unsigned, unsigned> > razmotavanja;
  std::map<BasicBlock *, std::vector<std::pair<unsigned, unsigned> > >::iterator
      ibb = lav::FLoopBlocks.find(BB);

  if (ibb != lav::FLoopBlocks.end()) {
    razmotavanja = ibb->second;
    obrisati.insert(ibb->first);
  }
  razmotavanja.push_back(
      std::pair<unsigned, unsigned>(loop_no, brRazmotavanja));

  lav::FLoopBlocks[New] = razmotavanja;
}

Function *CreateFChangeFunction() {
  llvm::ArrayRef<llvm::Type *> Params;
  FunctionType *ft = FunctionType::get(
      llvm::Type::getVoidTy(getGlobalContext()), Params, true);
  return Function::Create(ft, llvm::GlobalValue::ExternalLinkage, "FChange");
}

Function *CreateFIncFunction() {
  llvm::ArrayRef<llvm::Type *> Params;
  FunctionType *ft = FunctionType::get(
      llvm::Type::getVoidTy(getGlobalContext()), Params, true);
  return Function::Create(ft, llvm::GlobalValue::ExternalLinkage, "FInc");
}
Function *CreateFDecFunction() {
  llvm::ArrayRef<llvm::Type *> Params;
  FunctionType *ft = FunctionType::get(
      llvm::Type::getVoidTy(getGlobalContext()), Params, true);
  return Function::Create(ft, llvm::GlobalValue::ExternalLinkage, "FDec");
}

CallInst *CreateFIncCallInstruction(const std::set<Value *> &IncValues) {
  Function *f = CreateFIncFunction();
  std::vector<Value *> args;
  for (std::set<Value *>::iterator ii = IncValues.begin(), ie = IncValues.end();
       ii != ie; ++ii) {
    Value *param = *(ii);
    args.push_back(param);
  }

  // llvm::CallInst* llvm::CallInst::Create(llvm::Value*,
  // llvm::ArrayRef<llvm::Value*>, const llvm::Twine&, llvm::Instruction*)
  return CallInst::Create(f, args, "");
}

CallInst *CreateFDecCallInstruction(const std::set<Value *> &DecValues) {
  Function *f = CreateFDecFunction();
  std::vector<Value *> args;
  for (std::set<Value *>::iterator ii = DecValues.begin(), ie = DecValues.end();
       ii != ie; ++ii) {
    Value *param = *(ii);
    args.push_back(param);
  }
  return CallInst::Create(f, args, "");
}

CallInst *CreateFChangeCallInstruction(const std::set<Value *> &ChangedValues,
                                       const std::set<Value *> &IncValues,
                                       const std::set<Value *> &DecValues) {
  Function *f = CreateFChangeFunction();
  std::vector<Value *> args;

  for (std::set<Value *>::iterator ii = ChangedValues.begin(),
                                   ie = ChangedValues.end();
       ii != ie; ++ii) {
    Value *param = *(ii);
    if (IncValues.find(param) != IncValues.end())
      continue;
    if (DecValues.find(param) != DecValues.end())
      continue;
    args.push_back(param);
  }

  return CallInst::Create(f, args, "");
}

Function *CreateFEndFunction() {
  std::vector<Type *> Params;
  FunctionType *ft = FunctionType::get(
      llvm::Type::getVoidTy(getGlobalContext()), Params, true);
  return Function::Create(ft, llvm::GlobalValue::ExternalLinkage, "FEnd");
}

CallInst *CreateFEndCallInstruction() {
  Function *f = CreateFEndFunction();
  return CallInst::Create(f);
}

Function *CreateFUnrollFunction() {
  std::vector<Type *> Params;
  FunctionType *ft = FunctionType::get(
      llvm::Type::getVoidTy(getGlobalContext()), Params, true);
  return Function::Create(ft, llvm::GlobalValue::ExternalLinkage, "FUnroll");
}

std::string GetNameSufix(const std::string &name, int i) {
  char SuffixBuffer[100];
  sprintf(SuffixBuffer, ".%d", i);
  return (name + SuffixBuffer);
}

void CopyInstructionsFromBlock(BasicBlock *to, BasicBlock *from,
                               const std::string &NameSuffix) {
  ValueToValueMapTy ValueMap;
  for (BasicBlock::iterator lbit = from->begin(), lbie = from->end();
       lbit != lbie; ++lbit) {
    Instruction *newInst = lbit->clone();
    if (lbit->hasName())
      newInst->setName(lbit->getName() + NameSuffix);

    to->getInstList().push_back(newInst);
    ValueMap[lbit] = newInst;
  }

  // Remap all instructions in the most recent iteration
  for (BasicBlock::iterator I = to->begin(), E = to->end(); I != E; ++I)
    RemapInstruction1(I, ValueMap);

}

BasicBlock *CreateChangeBlock(const std::string &NameSuffix, Function *parent,
                              const std::set<Value *> &ChangedValues,
                              const std::set<Value *> &IncValues,
                              const std::set<Value *> &DecValues) {
  BasicBlock *New = BasicBlock::Create(getGlobalContext(), NameSuffix, parent);
  if (IncValues.size() != 0) {
    CallInst *calli1 = CreateFIncCallInstruction(IncValues);
    New->getInstList().push_back(calli1);
  }
  if (DecValues.size() != 0) {
    CallInst *calli1 = CreateFDecCallInstruction(DecValues);
    New->getInstList().push_back(calli1);
  }
  CallInst *calli =
      CreateFChangeCallInstruction(ChangedValues, IncValues, DecValues);
  New->getInstList().push_back(calli);
  return New;
}

void PhiExitBlocks(BasicBlock *BBlock, BasicBlock *newBBlock, Loop *L,
                   ValueToValueMapTy &VMap) {
  // Add phi entries for newly created values to all exit blocks.
  for (succ_iterator SI = succ_begin(BBlock), SE = succ_end(BBlock); SI != SE;
       ++SI) {
    if (L->contains(*SI))
      continue;
    for (BasicBlock::iterator BBI = (*SI)->begin();
         PHINode *phi = dyn_cast<PHINode>(BBI); ++BBI) {
      Value *Incoming = phi->getIncomingValueForBlock(BBlock);
      ValueToValueMapTy::iterator It = VMap.find(Incoming);
      if (It != VMap.end())
        Incoming = It->second;
      phi->addIncoming(Incoming, newBBlock);
    }
  }

}

void AddToMapDstSrc(ValueToValueMapTy &LastValueMap, ValueToValueMapTy &VMap) {
  for (ValueToValueMapTy::iterator VI = VMap.begin(), VE = VMap.end(); VI != VE;
       ++VI)
    LastValueMap[VI->first] = VI->second;
}

void TryToUnrollOnce(Loop *L, LoopInfo *LI, int It,
                     std::vector<BasicBlock *> &Headers,
                     std::vector<BasicBlock *> &Latches, BasicBlock *Header,
                     BasicBlock *LatchBlock, unsigned brRazmotavanja,
                     LoopBlocksDFS::RPOIterator &BlockBegin,
                     LoopBlocksDFS::RPOIterator &BlockEnd,
                     std::vector<PHINode *> &OrigPHINode) {
  std::vector<BasicBlock *> NewBlocks;
  ValueToValueMapTy LastValueMap;

  for (LoopBlocksDFS::RPOIterator BB = BlockBegin; BB != BlockEnd; ++BB) {

    ValueToValueMapTy VMap;
    BasicBlock *New = CloneBasicBlock(*BB, VMap, "." + Twine(It));
    Header->getParent()->getBasicBlockList().push_back(New);

    DodajObelezje(*BB, New, brRazmotavanja);

    // Loop over all of the PHI nodes in the block, changing them to use the
    // incoming values from the previous block.
    if (*BB == Header)
      for (unsigned i = 0, e = OrigPHINode.size(); i != e; ++i) {
        PHINode *NewPHI = cast<PHINode>(VMap[OrigPHINode[i]]);
        Value *InVal = NewPHI->getIncomingValueForBlock(LatchBlock);
        if (Instruction *InValI = dyn_cast<Instruction>(InVal))
          if (It > 1 && L->contains(InValI))
            InVal = LastValueMap[InValI];
        VMap[OrigPHINode[i]] = InVal;
        New->getInstList().erase(NewPHI);
      }

    // Update our running map of newest clones
    LastValueMap[*BB] = New;
    AddToMapDstSrc(LastValueMap, VMap);

    L->addBasicBlockToLoop(New, LI->getBase());

    // Add phi entries for newly created values to all exit blocks.
    PhiExitBlocks(*BB, New, L, LastValueMap);

    // Keep track of new headers and latches as we create them, so that
    // we can insert the proper branches later.
    if (*BB == Header)
      Headers.push_back(New);
    if (*BB == LatchBlock)
      Latches.push_back(New);

    NewBlocks.push_back(New);
  }

  // Remap all instructions in the most recent iteration
  RemapAllInstructions(NewBlocks, LastValueMap);

}

bool isBackEdgeBB(BasicBlock *bb, const std::vector<BasicBlock *> &BackEdges) {
  for (unsigned i = 0; i < BackEdges.size(); i++)
    if (BackEdges[i] == bb)
      return true;
  return false;
}

BranchInst *GetBI(BasicBlock *BB) {

  TerminatorInst *TermBB = BB->getTerminator();
  assert(TermBB != NULL);
  if (TermBB == NULL)
    return NULL;

  BranchInst *BIBB = dyn_cast<BranchInst>(TermBB);
  assert(BIBB != NULL);

  return BIBB;
}

bool SetSuccEnd(Loop *L, BranchInst *BI, BasicBlock *EndBlock) {
  if (BI->isUnconditional() != 1) {
    if (L->contains(BI->getSuccessor(0)))
      BI->setSuccessor(0, EndBlock);
    else
      BI->setSuccessor(1, EndBlock);
    return true;
  } else if (L->contains(BI->getSuccessor(0)))
    BI->setSuccessor(0, EndBlock);
  return true;
}

bool LastIteration(Loop *L, LoopInfo *LI, int It,
                   std::vector<BasicBlock *> &Headers,
                   std::vector<BasicBlock *> &Latches, BasicBlock *Header,
                   BasicBlock *LatchBlock, BasicBlock *EndBlock,
                   const std::vector<BasicBlock *> &BackEdges,
                   unsigned brRazmotavanja,
                   LoopBlocksDFS::RPOIterator &BlockBegin,
                   LoopBlocksDFS::RPOIterator &BlockEnd,
                   std::vector<PHINode *> &OrigPHINode) {

  ValueToValueMapTy LastValueMap;
  std::vector<BasicBlock *> NewBlocks;

  std::vector<BasicBlock *> NewBlocksDodatak;
  ValueToValueMapTy LastValueMapDodatak;

  //ovde bi trebalo da sve sto se vraca nazad u petlju bude preusmereno na zlaz
  //iz petlje
  for (LoopBlocksDFS::RPOIterator BB = BlockBegin; BB != BlockEnd; ++BB) {
    ValueToValueMapTy VMap1;
    BasicBlock *New = CloneBasicBlock(*BB, VMap1, "." + Twine(It));
    Header->getParent()->getBasicBlockList().push_back(New);

    DodajObelezje(*BB, New, brRazmotavanja);
    L->addBasicBlockToLoop(New, LI->getBase());

    // Loop over all of the PHI nodes in the block, changing them to use the
    // incoming values from the previous block.
    if (*BB == Header)
      for (unsigned i = 0, e = OrigPHINode.size(); i != e; ++i) {
        PHINode *NewPHI = cast<PHINode>(VMap1[OrigPHINode[i]]);
        Value *InVal = NewPHI->getIncomingValueForBlock(LatchBlock);
        if (Instruction *InValI = dyn_cast<Instruction>(InVal))
          if (It > 1 && L->contains(InValI))
            InVal = LastValueMap[InValI];
        VMap1[OrigPHINode[i]] = InVal;
        New->getInstList().erase(NewPHI);
      }

    if (isBackEdgeBB(*BB, BackEdges)) {
      BranchInst *BI = GetBI(New);
      if (BI == NULL)
        return false;

      //ako je bezuslovan skok
      if (BI->isUnconditional() == 1) {
        if (BI->getSuccessor(0) == Header) {

          ValueToValueMapTy VMap;
          BasicBlock *NewHeader =
              CloneBasicBlock(Header, VMap, "." + Twine(It + 1));
          Header->getParent()->getBasicBlockList().push_back(NewHeader);
          BranchInst *BIHeader = GetBI(NewHeader);

          //!      L->addBasicBlockToLoop(NewHeader, LI->getBase());

          //FIXME ovo moze da bude null ukoliko je u pitanju switch instrukcija
          //za grananje umesto br
          if (BIHeader == NULL) {
            llvm::outs() << *NewHeader;
            return false;
          }
          //      DodajObelezje(*BB, NewHeader, brRazmotavanja+1);

          //ovo ce da radi za Error: goto Error
          //ili za Error: goto Error1; Error1: goto Error
          //ali ne radi u oba slucaj ukoliko ima kod neki i grananja izmedju
          //za takav slucaj sada se vodi racuna da ne dodje do uzajamne
          //rekurzije na drugom mestu

          if (BIHeader->isUnconditional() == 1) {
            BIHeader->setSuccessor(0, EndBlock);
            BI->setSuccessor(0, NewHeader);
            LastValueMap[*BB] = NewHeader;
            NewBlocks.push_back(*BB);
            LastValueMapDodatak[Header] = EndBlock;
            NewBlocksDodatak.push_back(Header);
          } else {
            LastValueMapDodatak[*BB] = New;

            // Update our running map of newest clones
            LastValueMapDodatak[Header] = NewHeader;
            AddToMapDstSrc(LastValueMapDodatak, VMap);

            // Add phi entries for newly created values to all exit blocks.
            PhiExitBlocks(Header, NewHeader, L, LastValueMapDodatak);

            Headers.push_back(NewHeader);
            NewBlocksDodatak.push_back(NewHeader);

            // FIXME ukoliko imamo for sa 4 uslova, nece raditi for(i = 0; i<n
            // && j<k & m<n && a<b; i++ )
            if (L->contains(BIHeader->getSuccessor(0)) &&
                L->contains(BIHeader->getSuccessor(1))) {

              BasicBlock *succ0 = BIHeader->getSuccessor(0);
              BasicBlock *succ1 = BIHeader->getSuccessor(1);

              //            {//pocetak obrade succ0

              ValueToValueMapTy VMap2;
              BasicBlock *newSucc0 =
                  CloneBasicBlock(succ0, VMap2, "." + Twine(It + 1));
              succ0->getParent()->getBasicBlockList().push_back(newSucc0);
              BranchInst *BI0 = GetBI(newSucc0);
              if (BI0 == NULL)
                return false;

              //      DodajObelezje(*BB, newSucc0, brRazmotavanja+1);
              //!      L->addBasicBlockToLoop(newSucc0, LI->getBase());

              AddToMapDstSrc(LastValueMapDodatak, VMap2);
              NewBlocksDodatak.push_back(newSucc0);

              if (succ0 == LatchBlock)
                Latches.push_back(newSucc0);

              BIHeader->setSuccessor(0, newSucc0);

              LastValueMapDodatak[succ0] = newSucc0;
              VMap2[succ0] = newSucc0;

              //            }//kraj obrade succ0 bez phi

              { //pocetak obrade succ1

                ValueToValueMapTy VMap3;
                BasicBlock *newSucc1 =
                    CloneBasicBlock(succ1, VMap3, "." + Twine(It + 1));
                succ1->getParent()->getBasicBlockList().push_back(newSucc1);

                //      DodajObelezje(*BB, newSucc1, brRazmotavanja+1);
                //!      L->addBasicBlockToLoop(newSucc1, LI->getBase());

                BranchInst *BI1 = GetBI(newSucc1);
                if (BI1 == NULL)
                  return false;

                LastValueMapDodatak[succ1] = newSucc1;
                VMap3[succ1] = newSucc1;

                if (BI0->isUnconditional() == 1) {
                  BI0->setSuccessor(0, newSucc1);
                  LastValueMapDodatak[BI0->getSuccessor(0)] = newSucc1;
                  VMap2[BI0->getSuccessor(0)] = newSucc1;
                  VMap3[BI0->getSuccessor(0)] = newSucc1;
                }

                if (BI1->isUnconditional() == 1) {
                  BI1->setSuccessor(0, newSucc0);
                  LastValueMapDodatak[BI1->getSuccessor(0)] = newSucc0;
                  VMap2[BI1->getSuccessor(0)] = newSucc0;
                  VMap3[BI1->getSuccessor(0)] = newSucc0;
                }

                LastValueMapDodatak[BIHeader->getSuccessor(1)] = newSucc1;

                VMap2[succ1] = newSucc1;
                VMap2[succ0] = newSucc0;

                RemapBlockInstructions(newSucc0, VMap2);
                // Add phi entries for newly created values to all exit blocks.
                PhiExitBlocks(succ0, newSucc0, L, VMap2);

                if (BI0->isUnconditional() != 1)
                  if (BI0->getSuccessor(0) == LatchBlock ||
                      BI0->getSuccessor(1) == LatchBlock)
                    SetSuccEnd(L, BI0, EndBlock);
                if (BI1->isUnconditional() != 1)
                  if (BI1->getSuccessor(0) == LatchBlock ||
                      BI1->getSuccessor(1) == LatchBlock)
                    SetSuccEnd(L, BI1, EndBlock);
                //ovo je da se pokrije slucaj za tri bloka
                if (BI0->isUnconditional() != 1 &&
                    BI1->isUnconditional() != 1) {
                  BasicBlock *succ2;
                  if (L->contains(BI0->getSuccessor(0)))
                    succ2 = BI0->getSuccessor(0);
                  else
                    succ2 = BI0->getSuccessor(1);

                  ValueToValueMapTy VMap4;
                  BasicBlock *newSucc2 =
                      CloneBasicBlock(succ2, VMap4, "." + Twine(It + 1));
                  succ2->getParent()->getBasicBlockList().push_back(newSucc2);

                  //      DodajObelezje(*BB, newSucc2, brRazmotavanja+1);
                  //!      L->addBasicBlockToLoop(newSucc2, LI->getBase());

                  BranchInst *BI2 = GetBI(newSucc2);
                  if (BI2 == NULL)
                    return false;

                  LastValueMapDodatak[succ2] = newSucc2;
                  VMap4[succ2] = newSucc2;

                  if (L->contains(BI0->getSuccessor(0)))
                    BI0->setSuccessor(0, newSucc2);
                  else
                    BI0->setSuccessor(1, newSucc2);

                  AddToMapDstSrc(LastValueMapDodatak, VMap4);
                  NewBlocksDodatak.push_back(newSucc2);
                  LastValueMapDodatak[succ2] = newSucc2;

                  // Add phi entries for newly created values to all exit
                  // blocks.
                  PhiExitBlocks(succ2, newSucc2, L, VMap4);
                }

                if (succ1 == LatchBlock)
                  Latches.push_back(newSucc1);

                if (L->contains(BIHeader->getSuccessor(0)))
                  BIHeader->setSuccessor(0, newSucc1);
                else
                  BIHeader->setSuccessor(1, newSucc1);

                // Add phi entries for newly created values to all exit blocks.
                PhiExitBlocks(succ1, newSucc1, L, VMap3);

                AddToMapDstSrc(LastValueMapDodatak, VMap3);
                NewBlocksDodatak.push_back(newSucc1);

                RemapBlockInstructions(newSucc1, VMap3);

              } //kraj obrade succ1

            } else { //ako petlja ne sadrzi oba sledbenika bloka vec samo jedan
              SetSuccEnd(L, BIHeader, EndBlock);
            }
            BranchInst::Create(NewHeader, BI);
            New->getInstList().pop_back();

            ValueToValueMapTy LastValueMap1;
            LastValueMap1[Header] = NewHeader;
            AddToMapDstSrc(LastValueMap1, VMap);

            Headers.push_back(NewHeader);

            // Add phi entries for newly created values to all exit blocks.
            PhiExitBlocks(Header, NewHeader, L, LastValueMap1);
            RemapBlockInstructions(NewHeader, LastValueMap1);
          }

        } //else od if(BI->getSuccessor(0) == Header)
            else {
          std::cerr << "OVDE JE GRESKA 1, jao! " << std::endl;
          std::cout << "OVDE JE GRESKA 1, jao! " << std::endl;
          BranchInst::Create(EndBlock, BI);
          New->getInstList().pop_back();
        }

      }   //else jeste unconditional ali mu sledbenik nije header
          else
          //pretpostavka je da jedna od njih pokazuje na header - zato sto je to
          //backedge
          //FIXME SUMNJIVO
          {
        if (BI->getSuccessor(0) == Header)
          BI->setSuccessor(0, EndBlock);
        else
          BI->setSuccessor(1, EndBlock);
      }
    } //else: nije backedge prema tome ne treba nista posebno, samo je klonirana
      //u startu

    // Update our running map of newest clones
    LastValueMap[*BB] = New;
    AddToMapDstSrc(LastValueMap, VMap1);

    // Add phi entries for newly created values to all exit blocks.
    PhiExitBlocks(*BB, New, L, LastValueMap);

    // Keep track of new headers and latches as we create them, so that
    // we can insert the proper branches later.
    if (*BB == Header)
      Headers.push_back(New);
    if (*BB == LatchBlock)
      Latches.push_back(New);

    NewBlocks.push_back(New);
  }

  for (unsigned newbl = 0; newbl < NewBlocksDodatak.size(); newbl++)
    if (!L->contains(NewBlocksDodatak[newbl]))
      L->addBasicBlockToLoop(NewBlocksDodatak[newbl], LI->getBase());

  // Remap all instructions in the most recent iteration
  RemapAllInstructions(NewBlocks, LastValueMap);
  RemapAllInstructions(NewBlocksDodatak, LastValueMapDodatak);

  return true;
}

void AddLastInstructionFromBlock(BasicBlock *to, BasicBlock *from,
                                 const std::string &NameSuffix) {
  //Prepisuje se bezuslovni skok na petlju
  BasicBlock::iterator lbit = from->end();
  --lbit;
  //Instruction* newInst = lbit->clone(getGlobalContext());
  Instruction *newInst = lbit->clone();
  if (lbit->hasName())
    newInst->setName(lbit->getName() + NameSuffix);
  //llvm::outs() << "poslednja instrukcija iz bloka " << *newInst << '\n';
  to->getInstList().push_back(newInst);
  //ovo je verovatno suvisno
  ValueToValueMapTy ValueMap;
  ValueMap[lbit] = newInst;
  RemapInstruction1(newInst, ValueMap);
}

// At this point, the code is well formed.  We now do a quick sweep over the
// inserted code, doing constant propagation and dead code elimination as we
// go.
void CPandDCE(Loop *L, LoopInfo *LI) {
  const std::vector<BasicBlock *> &NewLoopBlocks = L->getBlocks();
  for (std::vector<BasicBlock *>::const_iterator BB = NewLoopBlocks.begin(),
                                                 BBE = NewLoopBlocks.end();
       BB != BBE; ++BB)
    for (BasicBlock::iterator I = (*BB)->begin(), E = (*BB)->end(); I != E;) {
      Instruction *Inst = I++;

      if (isInstructionTriviallyDead(Inst))
        (*BB)->getInstList().erase(Inst);
      else if (Value *V = SimplifyInstruction(Inst))
        if (LI->replacementPreservesLCSSAForm(Inst, V)) {
          Inst->replaceAllUsesWith(V);
          (*BB)->getInstList().erase(Inst);
        }
    }
}

void SetUpBranches(LoopInfo *LI, std::vector<BasicBlock *> &Headers,
                   std::vector<BasicBlock *> &Latches, bool NeedConditional,
                   bool ContinueOnTrue, LPPassManager *LPM) {
  // Now that all the basic blocks for the unrolled iterations are in place,
  // set up the branches to connect them.
  for (unsigned i = 0, e = Latches.size(); i != e; ++i) {
    // The original branch was replicated in each unrolled iteration.
    BranchInst *Term = cast<BranchInst>(Latches[i]->getTerminator());

    // The branch destination.
    unsigned j = (i + 1) % e;
    if (j == 0)
      continue;
    BasicBlock *Dest = Headers[j];

    if (NeedConditional) {
      // Update the conditional branch's successor for the following
      // iteration.
      Term->setSuccessor(!ContinueOnTrue, Dest);
    } else {

      //       BranchInst* bbi =
      BranchInst::Create(Dest, Term);

      Term->eraseFromParent();

      // Merge adjacent basic blocks, if possible.
      if (BasicBlock *Fold = FoldBlockIntoPredecessor(Dest, LI, LPM)) {

        std::replace(Latches.begin(), Latches.end(), Dest, Fold);
        std::replace(Headers.begin(), Headers.end(), Dest, Fold);
      }

    }
  }
}

//vrsi razmotavanje petlje u opstem slucaju
//FIXME moze da razmota jednom vise
bool LAVUnroll(Loop *L, unsigned Count, unsigned CountEnd, LoopInfo *LI,
               LPPassManager *LPM, BasicBlock *EndBlock) {

  BasicBlock *LatchBlock = L->getLoopLatch();
  if (!LatchBlock) {
    DEBUG(dbgs() << "  Can't unroll; loop exit-block-insertion failed.\n");
    return false;
  }

  BasicBlock *Preheader = L->getLoopPreheader();
  assert(Preheader != NULL);
  if (!Preheader) {
    DEBUG(dbgs() << "  Can't unroll; loop preheader-insertion failed.\n");
    return false;
  }

  // Loops with indirectbr cannot be cloned.
  if (!L->isSafeToClone()) {
    DEBUG(dbgs() << "  Can't unroll; Loop body cannot be cloned.\n");
    return false;
  }

  BasicBlock *Header = L->getHeader();
  //  BranchInst *BI = dyn_cast<BranchInst>(LatchBlock->getTerminator());

  if (LPM) {
    ScalarEvolution *SE = LPM->getAnalysisIfAvailable<ScalarEvolution>();
    if (SE)
      SE->forgetLoop(L);
  }

  std::vector<BasicBlock *> LoopBlocks = L->getBlocks();

  //  bool ContinueOnTrue = L->contains(BI->getSuccessor(0));
  //  BasicBlock *LoopExit = BI->getSuccessor(ContinueOnTrue);

  // For the first iteration of the loop, we should use the precloned values for
  // PHI nodes.  Insert associations now.
  std::vector<PHINode *> OrigPHINode;
  for (BasicBlock::iterator I = Header->begin(); isa<PHINode>(I); ++I) {
    OrigPHINode.push_back(cast<PHINode>(I));
  }

  /*****************************************************/
  // Figure out which basic blocks contain back-edges to the loop header.
  std::vector<BasicBlock *> BackedgeBlocks;
  for (pred_iterator I = pred_begin(Header), E = pred_end(Header); I != E; ++I)
    if (*I != Preheader)
      BackedgeBlocks.push_back(*I);

  /*****************************************************/

  //KOJE SE SVE VREDNOSTI MENJAJU U PETLJI
  //-----------------------------------------
  std::set<Value *> ChangedValues;
  std::set<Value *> IncValues;
  std::set<Value *> DecValues;
  if (CountEnd > 0) {
    GetChangedValues(L, ChangedValues);
    GetIncValues(L, IncValues);
    GetDecValues(L, DecValues);
  }

  //-----------------------------------------
  std::vector<BasicBlock *> Headers;
  std::vector<BasicBlock *> Latches;
  Headers.push_back(Header);
  Latches.push_back(LatchBlock);

  // The current on-the-fly SSA update requires blocks to be processed in
  // reverse postorder so that LastValueMap contains the correct value at each
  // exit.
  LoopBlocksDFS DFS(L);
  DFS.perform(LI);

  // Stash the DFS iterators before adding blocks to the loop.
  LoopBlocksDFS::RPOIterator BlockBegin = DFS.beginRPO();
  LoopBlocksDFS::RPOIterator BlockEnd = DFS.endRPO();

  unsigned brRazmotavanja = 0;

  //1. RAZMOTATI PETLJE COUNT PUTA
  //2. UBACITI BLOK KOJI MENJA SVE PROMENLJIVE
  //3. UBACITI NEKOLIKO IZVRSAVANJA PETLJE U KOJE SE DOLAZI IZ PRETHODNOG BLOKA
  //4. UBACITI NA SVAKOM IZLASKU DA IDE U ENDBLOCK

  //1.
  //-----------------------------------------------------------------------------
  //1. RAZMOTATI PETLJE COUNT PUTA
  if (Count > 0)
      //razmotati jednom manje, i onda se doda jos poslednje razmotavanje pa ima
      //ukupno dovoljno razmotavanja,
      {
    //ovo -1 je jer vec postoji jedno razmotavanje iz orginalnog koda na koje se
    //kace nova razmotavanja
    for (unsigned It = 1; It < Count - 1; ++It) {

      TryToUnrollOnce(L, LI, It, Headers, Latches, Header, LatchBlock,
                      brRazmotavanja, BlockBegin, BlockEnd, OrigPHINode);
      brRazmotavanja++;

    }

  }

  //1.-------------------------------------------------------------------------
  BasicBlock *New;
  std::string NameSuffix;
  Function *parent = Header->getParent();

  //FIXME Ako necemo poslednje razmotavanje onda necemo ni fchange blok - mozda
  //to drugacije podesiti
  if (CountEnd > 0) {

    //FIXME problem sa istim imenima namesuffix moze da bude isti ako je petlja
    //u petlji

    //2.
    //-----------------------------------------------------------------------------
    //2. UBACITI BLOK KOJI MENJA SVE PROMENLJIVE i prelazak na sledece
    //razmotavanje
    NameSuffix = GetNameSufix((*LoopBlocks.begin())->getName(), Count);
    New = CreateChangeBlock(NameSuffix, parent, ChangedValues, IncValues,
                            DecValues);

    //ovo bi verovatno moglo da se uradi jednostavnije - samo da se prepise
    //destinacija
    //podrazumeva da je ovo bezuslovan skok
    //ovde se radi reGetNameSufixmap
    AddLastInstructionFromBlock(New, LatchBlock, NameSuffix);

    L->addBasicBlockToLoop(New, LI->getBase());
    DodajObelezje(NULL, New, brRazmotavanja);

    //?
    Headers.push_back(New);
    Latches.push_back(New);
    //--------------------------------------------------------------------------------
  }

  //std::cout << "----------LAVUnroll -----------razmotavanje 7.5" << std::endl;
  //3.
  //-----------------------------------------------------------------------------
  //3. UBACITI IZVRSAVANJE PETLJE U KOJE SE DOLAZI IZ PRETHODNOG BLOKA
  if (CountEnd > 0)
    for (unsigned It = 1; It != CountEnd; ++It) {
      TryToUnrollOnce(L, LI, Count + It, Headers, Latches, Header, LatchBlock,
                      brRazmotavanja, BlockBegin, BlockEnd, OrigPHINode);
      brRazmotavanja++;
    }

  //-----------------------------------------------------------------------------
  //4. PREPISATI SVE BLOKOVE SA IZMENOM ZA BACKEDGE NA ENDBLOCK
  if (!LastIteration(L, LI, Count + CountEnd + 1, Headers, Latches, Header,
                     LatchBlock, EndBlock, BackedgeBlocks, brRazmotavanja,
                     BlockBegin, BlockEnd, OrigPHINode))
    std::cerr << "greska teska! u razmotavanju " << std::endl;
  //else std::cout << "last iteration dodato " <<std::endl;

  //-----------------------------------------------------------------------------
  //4. KRAJ
  //-----------------------------------------------------------------------------

  // Loop over the PHI nodes in the original block, setting incoming values.
  for (unsigned i = 0, e = OrigPHINode.size(); i != e; ++i) {
    PHINode *PN = OrigPHINode[i];
    PN->replaceAllUsesWith(PN->getIncomingValueForBlock(Preheader));
    Header->getInstList().erase(PN);
  }

  // Now that all the basic blocks for the unrolled iterations are in place,
  // set up the branches to connect them.
  SetUpBranches(LI, Headers, Latches, false, false, LPM);

  return true;
}

//vrsi razmotavanje petlje
bool FUnrollLoop(Loop *L, unsigned Count, unsigned CountEnd, LoopInfo *LI,
                 LPPassManager *LPM, BasicBlock *EndBlock) {
  assert(L->isLCSSAForm());

  BasicBlock *LatchBlock = L->getLoopLatch();
  BranchInst *BI = dyn_cast<BranchInst>(LatchBlock->getTerminator());

  if (!BI) {
    DEBUG(dbgs() << "  Can't unroll; loop not terminated by a branch.\n");
    return false;
  }

  //  if (BI->isUnconditional())
  LAVUnroll(L, Count, CountEnd, LI, LPM, EndBlock);
  //  else  UnrollSTDLoop(L, Count, CountEnd, LI, LPM, EndBlock);

  // At this point, the code is well formed.  We now do a quick sweep over the
  // inserted code, doing constant propagation and dead code elimination as we
  // go.
  CPandDCE(L, LI);

  // Remove the loop from the LoopPassManager if it's completely removed.
  //  if (LPM != NULL)
  //    LPM->deleteLoopFromQueue(L);

  if (LPM) {
    // FIXME: Reconstruct dom info, because it is not preserved properly.
    // Incrementally updating domtree after loop unrolling would be easy.
    if (DominatorTree *DT = LPM->getAnalysisIfAvailable<DominatorTree>())
      DT->runOnFunction(*L->getHeader()->getParent());

    LPM->deleteLoopFromQueue(L);

  }

  return true;
}

BasicBlock *CreateEndBlock(const std::string &name, llvm::Function *parent) {
  BasicBlock *EndBlock = BasicBlock::Create(getGlobalContext(), name, parent);
  CallInst *calli = CreateFEndCallInstruction();
  UnreachableInst *unreach = new UnreachableInst(getGlobalContext());
  EndBlock->getInstList().push_back(calli);
  EndBlock->getInstList().push_back(unreach);
  return EndBlock;
}

bool FLoopUnroll::runOnLoop(Loop *L, LPPassManager &LPM) {
  assert(L->isLCSSAForm());

  LoopInfo *LI = &getAnalysis<LoopInfo>();

  BasicBlock *Header = L->getHeader();
  Function *ParentFunction = Header->getParent();

  DEBUG(dbgs() << "Loop Unroll: F[" << Header->getParent()->getName()
               << "] Loop %" << Header->getName() << "\n");

  //FIXME ovo treba da je jedinstven blok za funkciju, ovako svaka petlja ima
  //svoj sto je glupo...
  std::string nnn = Header->getName();
  std::string names = std::string("EndBlock") + nnn;
  BasicBlock *EndBlock = CreateEndBlock(names, ParentFunction);

  loop_no++;

  //ool b = false;
  //if(UnrollLoopEnd == 0 && Fixed)
  //   b = NewUnrollLoop(L, UnrollLoopBegin, LI, &LPM);
  //if(!b)
  FUnrollLoop(L, UnrollLoopBegin, UnrollLoopEnd, LI, &LPM, EndBlock);

  // FIXME: Reconstruct dom info, because it is not preserved properly.
  DominatorTree *DT = getAnalysisIfAvailable<DominatorTree>();
  if (DT) {
    DT->runOnFunction(*ParentFunction);
    DominanceFrontier *DF = getAnalysisIfAvailable<DominanceFrontier>();
    if (DF)
      DF->runOnFunction(*ParentFunction);
  }

  return true;
}
