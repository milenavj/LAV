//===-- LFunction.cpp-----------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "lav/Internal/LBlock.h"
#include "lav/Internal/LBlockStore.h"
#include "lav/Internal/LFunction.h"
#include "lav/Internal/LModule.h"
#include "lav/Internal/LCommon.h"
#include "lav/Internal/LExpressionTransformation.h"
#include "lav/Internal/LConstraints.h"
#include "lav/Internal/LInstruction.h"

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/Support/CallSite.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/GlobalVariable.h"

#include "expression/output/SMTFormater.h"

extern llvm::cl::opt<bool> FindFirstFlawed;

namespace {
llvm::cl::opt<bool> CalculateAll(
    "check-function-conds",
    llvm::cl::desc("LAV --- Check in one solver call all conditions inside one "
                   "function (default = false)"),
    llvm::cl::init(false));

llvm::cl::opt<bool> CalculateBlock(
    "check-block-conds",
    llvm::cl::desc("LAV --- Check in one solver call all conditions inside one "
                   "block (default = false)"),
    llvm::cl::init(false));
}

namespace lav {
static argo::SMTFormater SMTF;

LFunction::LFunction(llvm::Function *f, LModule *p)
    : _Function(f), _Parent(p), _PostconditionIsSet(false),
      _DescriptionsCalculated(false), _ConditionsCalculated(false), _Context(-1),
      _VariableCounter(0), _FunctionName(f->getName()), _MemCounter(0) {
  init();
}

std::string LFunction::GetNameOfNextVariable() {
  return GetFunctionName() + "_" + VARNAME + ItoS(_VariableCounter++);
}

void LFunction::AddBlocks() {
  unsigned i = 0;
  llvm::Function::iterator bbIt, bbIe;
  //Dodavanje blokova koji imaju ime
  for (bbIt = _Function->begin(), bbIe = _Function->end(); bbIt != bbIe;
       ++bbIt) {
    llvm::BasicBlock *bb = bbIt;
    //FIXME Proveriti sta raditi sa blokovima koji nemaju ime???
    if (bb->hasName()) {
      _Blocks.push_back(new LBlock(bb, this));
      _LlvmToF[bb] = _Blocks[i++];
    }
  }
}

/// isUsedOutsideOfDefiningBlock - Return true if this instruction is used by
/// PHI nodes or outside of the basic block that defines it, or used by a
/// switch or atomic instruction, which may expand to multiple basic blocks.
static bool isUsedOutsideOfDefiningBlock(llvm::Instruction *I) {
  if (llvm::isa<llvm::PHINode>(I))
    return true;
  llvm::BasicBlock *BB = I->getParent();
  for (llvm::Value::use_iterator UI = I->use_begin(), E = I->use_end(); UI != E;
       ++UI)
    if (llvm::cast<llvm::Instruction>(*UI)->getParent() != BB ||
        llvm::isa<llvm::PHINode>(*UI))
      return true;
  return false;
}

LBlock *LFunction::GetLBlock(llvm::BasicBlock *bb) const {
  std::map<llvm::BasicBlock *, LBlock *>::const_iterator iter =
      _LlvmToF.find(bb);
  if (iter == _LlvmToF.end())
    return NULL;
  return iter->second;
}

LFunction::~LFunction() {
  for (unsigned i = 0; i < _Blocks.size(); i++)
    delete _Blocks[i];
}

void LFunction::SetReturnInstructions() {
  for (llvm::inst_iterator II = inst_begin(_Function), E = inst_end(_Function);
       II != E; ++II) {
    llvm::Instruction *instr = &*II;
    if (instr->getOpcode() == llvm::Instruction::Ret)
      _ReturnInstructions.push_back(instr);
  }
}

//bool LFunction::AddAllocationFixedAddress(llvm::AllocationInst *ai,
//std::vector<std::string>& variables, std::vector<LVariableInfo>& fvis )
bool LFunction::AddAllocationFixedAddress(llvm::AllocaInst *ai,
                                          std::vector<std::string> &variables,
                                          std::vector<LVariableInfo> &fvis) {

  const llvm::Type *t = ai->getAllocatedType();

  if (t && t->isSized() && t->isAggregateType()) {
    //Ovo je alokacija, treba joj staviti fiksnu adresu
    int size = GetParentModule()->GetTargetData()
        ->getTypeAllocSize(ai->getAllocatedType());

    //FIXME ovo je teska krpljevina
    //treba ove adrese da se dodaju na drugom mestu jer ako nema poziva funkcije
    //onda se one ne dodaju a
    //to je onda pogresno
    //FIXME *1000 je krpljevina da se adrese ne bi gadjale ako neko prekoraci
    //tacno toliko onda je greska
    //u todo ima bolje resenje za ovo
    argo::Expression e = ExpAddress(GetFunctionName(), GetOperandName(ai));
    if (_Addresses.find(GetOperandName(ai)) == _Addresses.end())
      _Addresses.insert(
          std::pair<std::string, unsigned>(GetOperandName(ai), size * 1000));
    else
      _Addresses[GetOperandName(ai)] = size;
    std::cout << "function::AddAllocationFixedAddress" << std::endl;
    FunctionConstraints().AddConstraint(e, 0, size);

    variables.push_back(GetOperandName(ai));
    LVariableInfo fvi(new argo::Expression(e), ai->getAllocatedType(), false);
    fvis.push_back(fvi);
    return true;
  }
  return false;
}

void LFunction::AddReferenceConstraint(llvm::AllocaInst *ai) {
  //  if(_References.find(GetOperandName(ai)) != _References.end())
  //  {
  llvm::Type *t = ai->getAllocatedType();

  //FIXME ovo treba i za pointere
  //ako se negde uzima referenca od ovog int-a onda se ne ubacuje u store
  //ali se ubacuje left i right

  // ovo vraca sve poravnato na prvu vecu vrednost deljivu sa 4
  // ako je struktura char[10], int ovo vrati 16
  // ako je unija char[10], int ovo vrati 12
  unsigned size = GetParentModule()->GetTargetData()->getTypeAllocSize(t);
  argo::Expression a = ExpAddress(GetFunctionName(), GetOperandName(ai));
  //  std::cout << "function::AddReferenceConstraint" << std::endl;
  FunctionConstraints().AddConstraint(a, 0, size);
  //  }
}

void LFunction::AddGlobalVariables(std::vector<std::string> &variables,
                                   std::vector<LVariableInfo> &fvis) {
  std::set<llvm::GlobalVariable *> GlobalVariables =
      GetParentModule()->GlobalVariables();
  std::set<llvm::GlobalVariable *>::const_iterator i = GlobalVariables.begin(),
                                                   ie = GlobalVariables.end();
  std::set<llvm::GlobalVariable *> &GlobalReferences =
      GetParentModule()->GlobalReferences();
  //FIXME da li dodavati u store nizove i pokazivace --- mozda ne ---

  for (; i != ie; i++) {
    //ako je pointer - ne pratimo ga vec preko memorije
    //ako je niz ili struktura - isto preko memorije
    llvm::GlobalVariable *gv = *i;
    const llvm::SequentialType *stype =
        llvm::dyn_cast<llvm::SequentialType>(gv->getType());
    const llvm::Type *t = stype->getElementType();
    if (llvm::dyn_cast<llvm::PointerType>(t))
      continue;
    if (t->isSized() && t->isAggregateType())
      continue;

    if (GlobalReferences.find(gv) == GlobalReferences.end()) {
      variables.push_back(GlobalName(GetOperandName(gv)));
      LVariableInfo fvi(NULL, gv->getType(), true);
      fvis.push_back(fvi);
    }
  }
}
void LFunction::AddLocalVariables(std::vector<std::string> &variables,
                                  std::vector<LVariableInfo> &fvis) {

  for (llvm::inst_iterator II = inst_begin(_Function), E = inst_end(_Function);
       II != E; ++II) {
    //      ovo uvede gresku za neke primere za z3  i uspori neke primere
    //      ali resi problem modela u slucaju da imamo samo jedan blok
    //      bool isUsed;
    //       if(_Blocks.size() <= 2) isUsed = true;
    //          else isUsed = isUsedOutsideOfDefiningBlock(&*II);

    bool isUsed = isUsedOutsideOfDefiningBlock(&*II);
    if (!isUsed) {
      if (llvm::AllocaInst *ai = llvm::dyn_cast<llvm::AllocaInst>(&*II)) {
        if (_References.find(GetOperandName(ai)) != _References.end()) {
          AddReferenceConstraint(ai);
          continue;
        }
        AddAllocationFixedAddress(ai, variables, fvis);
      }
    }

    if (isUsed) {
      //FIXME: sta se desava za malloc - proveriti
      if (llvm::AllocaInst *ai = llvm::dyn_cast<llvm::AllocaInst>(&*II)) {
        if (_References.find(GetOperandName(ai)) != _References.end()) {
          AddReferenceConstraint(ai);
          continue;
        }

        bool added = AddAllocationFixedAddress(ai, variables, fvis);
        if (!added) {
          variables.push_back(GetOperandName(ai));
          LVariableInfo fvi(NULL, ai->getAllocatedType(), true);
          fvis.push_back(fvi);
        }
      } else {
        if (_References.find(GetOperandName(&*II)) != _References.end()) {
          //FIXME videti sta raditi u ovom slucaju
          //std::cerr << "\n\n\n\n\n\n\n\n\nnije allocation a uzima se referenca
          //?!?" << GetOperandName(&*II) << std::endl;
          continue;
        }
        variables.push_back(GetOperandName(&*II));
        LVariableInfo fvi(NULL, II->getType(), true);
        fvis.push_back(fvi);
      }
    } //kraj od if(used)
  }   //kraj for-a

  //dodaje se na kraj mem varijabla
  variables.push_back(MEMORY);
  LVariableInfo fvi(NULL, NULL, true);
  fvis.push_back(fvi);
}

void LFunction::AddFunctionArguments(std::vector<std::string> &variables,
                                     std::vector<LVariableInfo> &fvis) {

  for (llvm::Function::arg_iterator I = _Function->arg_begin(),
                                    E = _Function->arg_end();
       I != E; ++I) {
    variables.push_back(GetOperandName(I));
    //ovime se znacajno gubi na efikasnosti,
    //jer najcesce se argumenti javljaju samo u entry bloku,
    //ovo su specijalni slucajevi kada se javljaju i kasnije i
    //trebalo bi takve prepoznati i samo u tim slucajevima
    //prepovezivati i dodati
    //LVariableInfo fvi(NULL, I->getType(), false);
    LVariableInfo fvi(NULL, I->getType(), true);
    fvis.push_back(fvi);
  }
}

void LFunction::SetAddresses() {
  //FIXME mozda raditi ovo isto prilikom skupljanja referenci, a mozda i ne
  //za malloc se radi kod referenci
  // za nizove se radi u set storu
  std::map<std::string, llvm::Type *>::iterator it = _References.begin(),
                                                ie = _References.end();
  for (; it != ie; ++it) {
    //FIXME AKO NEKO PREKORACI TACNO SIZE*1000 ONDA DOLAZI DO GRESKE
    unsigned size =
        GetParentModule()->GetTargetData()->getTypeAllocSize(it->second) * 1000;
    if (_Addresses.find(it->first) == _Addresses.end()) {
      _Addresses.insert(std::pair<std::string, unsigned>(it->first, size));
    }
  }
}

void LFunction::GetAddresses(unsigned &current,
                             std::vector<argo::Expression> &eqs) const {
  std::map<std::string, unsigned>::const_iterator it = _Addresses.begin(),
                                                  ie = _Addresses.end();
  for (; it != ie; ++it) {
    //TODO zbog free dodati proveru da li je second nula
    argo::Expression e;
    if (isMalloc(it->first)) {
      argo::Expression e1 =
          argo::Expression::Equality(ExpVar(it->first, fpointer_type, false),
                                     ExpNum1(current, fpointer_type));
      argo::Expression e2 = argo::Expression::Equality(
          ExpVar(it->first, fpointer_type, false), ExpNumZeroPtr);
      e = argo::Expression::OR(e1, e2);
    } else if (isMalloc2(it->first)) {
      argo::Expression e1 =
          argo::Expression::Equality(ExpVar(it->first, fpointer_type, false),
                                     ExpNum1(current, fpointer_type));
      argo::Expression e2 = argo::Expression::Equality(
          ExpVar(it->first, fpointer_type, false), ExpNumZeroPtr);
      e = argo::Expression::OR(e1, e2);
    } else if (isAddress(it->first))
      e = argo::Expression::Equality(ExpVar(it->first, fpointer_type, false),
                                     ExpNum1(current, fpointer_type));
    else
      e = argo::Expression::Equality(ExpAddress(GetFunctionName(), it->first),
                                     ExpNum1(current, fpointer_type));

    current += it->second;
    eqs.push_back(RenameExpressionVariables(e, _Context, GetFunctionName()));
  }
}

void LFunction::AddGlobalReferenceConstraint(llvm::GlobalVariable *gv) {
  if (GetParentModule()->GlobalReferences().find(gv) !=
      GetParentModule()->GlobalReferences().end())
    return;
  GetParentModule()->GlobalReferences().insert(gv);
  llvm::Type *t = gv->getType();
  unsigned size = GetParentModule()->GetTargetData()->getTypeAllocSize(t);
  argo::Expression e = ExpGlobalAddress(GetOperandName(gv));

  //  std::cout <<std::endl <<"GetFunctionName() "<<  GetFunctionName() <<
  // std::endl;
  //  std::cout << "function::AddGlobalReferenceConstraint " << " e.GetName() "
  // << e.GetName() << " left = 0 " << "right = " << size << std::endl;

  //!@#$ otkomentarisati i resiti problem

  GetParentModule()->GlobalConstraints().AddConstraint(e, 0, size);
  //FIXME nedostaje pomeraj*1000 --- ovo bi sve trebalo modul da radi
  GetParentModule()->_Addresses
      .insert(std::pair<std::string, unsigned>(e.GetName(), size));

  GetParentModule()->GlobalMemoryOffset() =
      GetParentModule()->GetGlobalMemoryOffset() + size;
}

void LFunction::AddReferences(llvm::Instruction *instr, unsigned op) {

  llvm::AllocaInst *ai =
      llvm::dyn_cast<llvm::AllocaInst>(instr->getOperand(op));
  llvm::GlobalVariable *gv =
      llvm::dyn_cast<llvm::GlobalVariable>(instr->getOperand(op));

  //FIXME dubiozno
  if (instr->getOperand(op)->hasName() && (ai || gv)) {
    llvm::GlobalVariable *gv =
        llvm::dyn_cast<llvm::GlobalVariable>(instr->getOperand(op));
    if (gv) {
      AddGlobalReferenceConstraint(gv);
      return;
    }

    std::string name = GetOperandName(instr->getOperand(op));
    llvm::Type *t = instr->getOperand(op)->getType();
    _References.insert(std::pair<std::string, llvm::Type *>(name, t));
  }
}

//FIXME glupo je da funckija postavlja global constraints - to bi trebao da radi
//modul!
void LFunction::SetReferences() {

  //za koje sve promenljive se uzima referenca
  for (llvm::inst_iterator II = inst_begin(_Function), E = inst_end(_Function);
       II != E; ++II) {
    llvm::Instruction *instr = &*II;

    if (instr->getOpcode() == llvm::Instruction::GetElementPtr &&
        instr->getOperand(0)->hasName()) {
      llvm::AllocaInst *ai =
          llvm::dyn_cast<llvm::AllocaInst>(instr->getOperand(0));
      if (ai) {
        if (instr->getOperand(0)->getType() != ai->getAllocatedType()) {
          //          AddReferenceConstraint(ai);
          AddReferences(instr, 0);
        }
      }
    }
    //fixme ovde se nekada doda referenca koja nije potrebna, nije mi jasno
    //zasto se ovo uopste dodaje
    if (instr->getOpcode() == llvm::Instruction::Store) {
      AddReferences(instr, 0);
    }

    //reference u operaciji poredjenja
    if (instr->getOpcode() == llvm::Instruction::ICmp ||
        instr->getOpcode() ==
            llvm::Instruction::
                FCmp) { //bojim se da ovde moze da bude nesto pogresno sa
                        //tipovima, npr da se dobije tip pokazivaca a da treba
                        //tip onoga na sta pokazivac pokazuje
      AddReferences(instr, 0);
      AddReferences(instr, 1);
    }

    //uzimanje reference u pozivu funkcije
    if ((instr->getOpcode() == llvm::Instruction::Call) ||
        (instr->getOpcode() == llvm::Instruction::Invoke)) {
      unsigned numArgs;
      llvm::Function *f = GetFunction(instr, numArgs);

      if (f && !(f->isDeclaration())) {
        for (unsigned j = 0; j < numArgs; ++j)
          AddReferences(instr, j);
      }

      //FIXME sta ako je malloc za globalnu promenljivu
      //FIXME ili ostale adrese ovde staviti ili ovo izmestiti negde drugde
      if (f && f->isDeclaration()) {
        if (f->getIntrinsicID() == llvm::Intrinsic::not_intrinsic)
          if (f->getName() == "malloc") {
            //TODO dodati i proveru tipa da je to bas the malloc a ne neki drugi
            _Addresses.insert(std::pair<std::string, unsigned>(
                MALLOC + GetOperandName(instr), 100000));
          }
      }
    } //kraj if-a za funkciju

  } //kraj fora

}

void LFunction::AddFixedAddresses(LFunction *ff) {
  std::string ff_function_name = GetFunctionName();

  std::map<std::string, unsigned>::iterator it = Addresses().begin(),
                                            ite = Addresses().end();
  std::string name;
  for (; it != ite; ++it) {
    if (isGlobalAddress(it->first))
      name = it->first;
    else if (isMalloc(it->first))
      name = AddContext(it->first, GetContext(), GetFunctionName());
    else if (isAddress(it->first))
      name = AddContext(it->first, GetContext(), GetFunctionName());
    else
      name = AddContext(AddressName(ff_function_name, it->first), GetContext(),
                        GetFunctionName());
    ff->Addresses().insert(std::pair<std::string, unsigned>(name, it->second));
  }
}

//Postavljanje storova u svakom bloku
void LFunction::SetStoresInBlocks() {
  std::vector<std::string> variables;
  std::vector<const llvm::Type *> types;
  std::vector<LVariableInfo> fvis;

  AddLocalVariables(variables, fvis);
  AddGlobalVariables(variables, fvis);
  AddFunctionArguments(variables,
                       fvis); //ovime se gubi efikasnost, argumenti se obicno
  //pojavljuju samo u prvom bloku, glupo je vuci ih svuda

  for (unsigned i = 0; i < _Blocks.size(); i++) {
    _Blocks[i]->AddStore(variables, fvis);
  }
}

void LFunction::init() {
  LBlock::ResetBlockNumber();
  AddBlocks();
  SetReferences();
  SetAddresses();
  SetStoresInBlocks();
  SetReturnInstructions();
}

std::ofstream &LFunction::PrintHTMLHeader(std::ofstream &f) const {

  std::string type_str;
  llvm::raw_string_ostream rso(type_str);
  _Function->getType()->print(rso);

  std::string s = _Function->getName();
  f << "<hr>";
  f << "<font face = \"Courier New\" size = \"2\"> function name: <tt>" << s
    << "</tt><br> type: <tt>"
      //<< *_Function->getType()
    << rso.str() << "</tt></font></h4>" << std::endl;
  return f;
}

void LFunction::PrintHTMLHeader(const std::string &sFilename) const {
  std::ofstream f(sFilename.c_str(), std::ios::app);
  if (!f.is_open()) {
    perror(
        ((std::string) "LFunction::PrintHTMLHeader Could not open html file " +
         sFilename));
    return;
  }
  PrintHTMLHeader(f);
  f.close();

}

std::ofstream &LFunction::PrintHTML(std::ofstream &f) const {

  PrintHTMLHeader(f);
  for (unsigned i = 0; i < _Blocks.size(); i++)
    _Blocks[i]->PrintHTML(f);

  return f;
}

std::ostream &LFunction::Print(std::ostream &ostr) const {
  for (unsigned i = 0; i < _Blocks.size(); i++)
    _Blocks[i]->Print(ostr);
  return ostr;
}

void LFunction::CalculateAllConditions() {
  if (_ConditionsCalculated)
    return;

  if (!_DescriptionsCalculated)
    CalculateDescriptions();

  LSolver::instance().reset();
  LSolver::instance().AddIntoSolver(GetParentModule()->GetGlobalConstraints());
  LSolver::instance().AddIntoSolver(GetFunctionConstraints());
  LSolver::instance().AddIntoSolver(_Blocks[0]->Active());

  std::vector<LLocalCondition *> conds;
  for (unsigned i = 0; i < _Blocks.size(); i++) {
    _Blocks[i]->AddPostconditionToSolver();
    _Blocks[i]->GetAllConditions(conds);
  }
  _Blocks[_Blocks.size() - 1]->UpdateAndSetAddresses();

  aExp F = aExp::TOP();
  STATUS s = LSolver::instance().callSolverBlock(F, conds);

  if (s != SAFE && FindFirstFlawed)
    for (unsigned i = 0; i < conds.size(); i++)
      if (conds[i]->Instruction()->GetParentBlock()->stopWhenFound(
              conds[i]->Instruction(), conds[i]->Status(), true) == -1)
        exit(1);

  _ConditionsCalculated = true;
  LSolver::instance().reset();

}

void LFunction::CalculateConditions() {

  if (_ConditionsCalculated)
    return;

  if (!_DescriptionsCalculated)
    CalculateDescriptions();

  LSolver::instance().reset();

  if (CalculateAll)
    CalculateAllConditions();
  else if (CalculateBlock) {
    LSolver::instance().AddIntoSolver(
        GetParentModule()->GetGlobalConstraints());
    LSolver::instance().AddIntoSolver(GetFunctionConstraints());
    for (unsigned i = 0; i < _Blocks.size(); i++)
      _Blocks[i]->CalculateConditionsBlock();
  } else if (_Blocks.size() > 7) {
    LSolver::instance().AddIntoSolver(
        GetParentModule()->GetGlobalConstraints());
    LSolver::instance().AddIntoSolver(GetFunctionConstraints());
    for (unsigned i = 0; i < _Blocks.size(); i++)
      _Blocks[i]->CalculateConditionsIncremental();
  } else {
    for (unsigned i = 0; i < _Blocks.size(); i++)
      _Blocks[i]->CalculateConditions();
  }
  _ConditionsCalculated = true;
  LSolver::instance().reset();
}

unsigned LFunction::GetLoopMax(unsigned loop) {
  std::map<unsigned, unsigned>::iterator it = _LoopMax.find(loop);
  if (it != _LoopMax.end())
    return it->second;
  return 0;
}

void LFunction::AddLoopMax(const std::pair<unsigned, unsigned> &loop) {
  unsigned petlja = loop.first;
  unsigned raz = loop.second;

  std::map<unsigned, unsigned>::iterator it = _LoopMax.find(petlja);
  if (it != _LoopMax.end()) {
    if (it->second < raz)
      it->second = raz;
  } else
    _LoopMax.insert(std::pair<unsigned, unsigned>(petlja, raz));

}

void LFunction::Run() {

  CalculateDescriptions();

  CalculateConditions();
}

void LFunction::CalculateDescriptions() {
  //test primeri prolaze i bez ovog reset, mozda je visak
  LSolver::instance().reset();
  std::vector<unsigned> desc;
  if (_DescriptionsCalculated)
    return;
  for (unsigned i = 0; i < _Blocks.size(); i++) {
    _Blocks[i]->CalculateDescriptions();
  }
  _DescriptionsCalculated = true;
}

argo::Expression LFunction::GetPostcondition() {
  if (!_PostconditionIsSet) {
    SetPostcondition();
}
   
  std::string cont = AddContext("", _Context, GetFunctionName());
  return RenameExpressionVariables(_Postcondition, cont, _Context);
}

void LFunction::BlocksPostcondInSolverSetFalse() {
  for (unsigned i = 0; i < _Blocks.size(); i++)
    _Blocks[i]->PostconditionInSolver() = false;
}

void LFunction::SetPostcondition() {
  if (_PostconditionIsSet)
    return;
  if (!_DescriptionsCalculated)
    CalculateDescriptions();

  std::vector<argo::Expression> blocksPostconditions;

  const LBlock *fb = GetLBlock(&(_Function->getEntryBlock()));
  assert(fb != NULL);
  blocksPostconditions.push_back(ActiveBlock(_Function->getName(), fb->Id()));

  for (unsigned i = 0; i < _Blocks.size(); i++)
    blocksPostconditions.push_back(_Blocks[i]->Postcondition());

  blocksPostconditions.push_back(GetFunctionConstraints());
  _Postcondition = argo::Expression::AND(blocksPostconditions);

  _PostconditionIsSet = true;
}

} //end of namespace
