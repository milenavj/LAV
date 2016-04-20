//===-- LBlock.cpp --------------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "lav/Internal/LBlock.h"
#include "lav/Internal/LFunction.h"
#include "lav/Internal/LModule.h"
#include "lav/Internal/LInstruction.h"
#include "lav/Internal/LCommon.h"
#include "lav/Internal/InstructionInfoTable.h"
#include "lav/Internal/LExpressionTransformation.h"
#include "lav/Misc/misc.h"

#include <ostream>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <fstream>
#include <tr1/memory>
#include <map>
#include <vector>

#include "llvm/IR/Type.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/GetElementPtrTypeIterator.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Support/CallSite.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/ValueSymbolTable.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Timer.h"
#include "llvm/Support/raw_ostream.h"

#include "expression/output/SMTFormater.h"
#include "expression/expressions/NumeralTypes.h"


#include "lav/Threads/FixedQueue.h"
#include "lav/Threads/ThreadPool.h"
using namespace ThreadSafe;
using namespace Utils;

namespace {

  llvm::cl::opt<bool>
  CalculateBlock("check-block-conds",
               llvm::cl::desc("LAV --- Check in one solver call all conditions inside one block (default = false)"),
               llvm::cl::init(false));
  llvm::cl::opt<int>
               NumberThreads("number-threads",
                            llvm::cl::desc("LAV --- Number of threads (default = hardware_concurrency)"),
                            llvm::cl::init(0));
  llvm::cl::opt<bool>
              EnableParallel("enable-parallel",
                           llvm::cl::desc("LAV --- Enable parallel solver calls (default = false)"),
                           llvm::cl::init(false));

}

extern llvm::cl::opt<bool> Model;
extern llvm::cl::opt<bool> FindFirstFlawed;
extern llvm::cl::opt<bool> CheckPointers;
extern llvm::cl::opt<bool> SkipInsideLoop;
extern llvm::cl::opt<bool> CheckDivZero;
extern llvm::cl::opt<bool> PrintHtml;
extern llvm::cl::opt<std::string> StartFunction;
extern llvm::cl::opt<std::string> InputFile;
extern llvm::cl::opt<std::string> OutputFolder;


namespace lav
{
extern std::map< llvm::BasicBlock*, vLoop> FLoopBlocks;
static argo::SMTFormater SMTF;

//llvm::Timer TryMergeTimer("TryMerge Time");
//llvm::Timer SetStoreTimer("SetStore Time");
//llvm::Timer SetPredsTimer("SetPreds Time");
//llvm::Timer SetJumpsTimer("SetJumps Time");
//llvm::Timer ChangeInitStoreTimer("ChangeInitStore Time");
//llvm::Timer UpdateStoreTimer("UpdateStore Time");
llvm::Timer AddLocalConditionTimer("AddLocalCondition Time");
llvm::Timer InlineTimer("Inline Time");
llvm::Timer ConnectCondsTimer("Connect conditions Time");
llvm::Timer AddPostCondTimer("Add function postcodn Time");
llvm::Timer BindTasksForThreads("Add task functions to queue for threads time");
llvm::Timer ParallelExecution("Parallel execution time");
llvm::Timer ffTimer("ff Time");

//////////////////////////////////////////////////////////////
// LLocalCondition
//////////////////////////////////////////////////////////////
std::ostream& LLocalCondition::Print(std::ostream& ostr) const
{
  _Instruction->Print(ostr);
  if(_Status == SAFE)           ostr << "SAFE "; else
  if(_Status == FLAWED)         ostr << "FLAWED"; else
  if(_Status == UNSAFE)         ostr << "UNSAFE "; else
  if(_Status == ERROR)          ostr << "ERROR "; else
  if(_Status == UNREACHABLE)    ostr << "UNREACHABLE"; else
  if(_Status == UNCHECKED)      ostr << "UNCHECKED"; else ostr << "???";
  ostr << std::endl;

  return ostr;
}

std::ostream& LLocalCondition::PrintHTML(std::ostream& f) const
{
  _Instruction->PrintHTML(f);
/*  f << "(cond && LHS) => RHS <br>" << std::endl;
  f << "LHS: <br>";
  _LHS.Print(&SMTF, f);
  f << std::endl;
  f << "<br>RHS: <br>";
  _RHS.Print(&SMTF, f);*/

  if(_Status == UNSAFE || _Status == FLAWED)
  {
    if(Model)
    {
    f  << "</tt><br>"<< std::endl;
    std::string s = ExtractFileName(_Instruction->GetModelFileName());
    f << "<tt><a href=\"" << s <<"\">" << "Model" << "</a>";
    }
    f  << "</tt><br></font> "<< std::endl;
  }

  f << std::endl << "<font face = \"Courier New\"> status: <tt>";
  if(_Status == SAFE)     f << "<font color = \"#008080\">SAFE </font>"; else
  if(_Status == FLAWED)   f << "<font color = red>FLAWED</font>"; else
  if(_Status == UNSAFE)   f << "<font color = brown>UNSAFE </font>"; else
  if(_Status == ERROR)    f << "<font color = red>ERROR </font>"; else
  if(_Status == UNREACHABLE)      f << "<font color = orange>UNREACHABLE </font>"; else
  if(_Status == UNCHECKED)        f << "<font color = blue> UNCHECKED</font>"; else f << "???";
  f << "</tt> </font ><br>" << std::endl;

  return f;
}

//////////////////////////////////////////////////////////////
// LJump
//////////////////////////////////////////////////////////////
std::ostream& LJump::Print(std::ostream& ostr) const
{
  ostr << "Jump: " ;
  _Condition.Print(&SMTF, ostr);
  ostr << " BB: " << _BB;
  if(_BB)
    {
    std::string s = _BB->getName();
    ostr << " BB->name: " << s;
    }
  if(_FB)
    ostr << " FB.ID(): " << _FB->Id();
  return ostr;
}


//////////////////////////////////////////////////////////////
// LBlock
//////////////////////////////////////////////////////////////

LBlock::LBlock(llvm::BasicBlock *bb, LFunction* parent)
                        :
                          _Id(BlockNumber++),
                          _BBlock(bb),
                          _Parent(parent),
                          _State(this),
                          _PredsSet(false),
                          _TraceCalculated(false),
                          _DescriptionsCalculated(false),
                          _ConditionsCalculated(false),
                          _StateIsSet(false),
                          _PostconditionIsSet(false),
                          _PostconditionInSolver(false),
                          _ChangeInitStore(false),
                          _HasNoReturnFC(false),
                          _HasAssume(false),
                          _Reachable(BLOCK_UNCHECKED)
{ init(); }




unsigned LBlock::BlockNumber = 0;

LBlock::~LBlock()
{
for(unsigned i=0; i<_Instructions.size(); i++)
    delete _Instructions[i];
}

std::ostream& LBlock::Print(std::ostream& ostr) const
{
  for(unsigned i=0; i<_LocalConditions.size(); i++)
    _LocalConditions[i].Print(ostr);
  return ostr;
}

std::ostream& LBlock::PrintHTML (std::ostream& f) const
{
  if(_LocalConditions.size()==0)
    return f;

  for(unsigned i=0; i<_LocalConditions.size(); i++)
  {
    f << "<p><font face = \"Courier New\" size = \"2\"> command: "
      << std::endl;
    _LocalConditions[i].PrintHTML(f);
    f << std::endl;
  }

  return f;
}



//ovo je jako skupo ako ima puno uslova
//mozda bi ga trebalo spakovati nekako u jedan uslov
//ili ga nekako inkrementalno ubrzati
void LBlock::ConnectFunctionConditions(LInstruction* fi, LFunction* ff)
{

  //ispitaj sve local conditions iz funkcije koje su bile unsafe
  const vpBlock& fLBlocks = ff->GetBlocks();

  for(unsigned k = 0; k<fLBlocks.size(); k++)
  {

    if(SkipInsideLoop)
    {
        const LBlock* fb = fLBlocks[k];
        if(fb->IsInsideLoop() && fb->DoNotCalculate())
        {
        //FIXME ovde bi trebalo sacuvati koje se ne proveravaju i izbrisati ih iz niza da se ne cuvaju
        continue;
        }
    }


    //uzimamo sve lokalne condition-e za taj blok
    const vLocCond& lc = fLBlocks[k]->GetLocalConditions();

    //pravimo lhs1 samo jednom i to prvi put kada zatreba
    aExp lhs1;
    int b=1;
    for(unsigned m = 0; m<lc.size(); m++)
    {

    //proveravaju se oni uslovi koji nisu safe

//    if((lc[m].Status()==UNSAFE) || (lc[m].Status()==UNCHECKED) || (lc[m].Status()==ERROR)) {
    if((lc[m].Status()==UNSAFE)  || (lc[m].Status()==ERROR)) {


    //ako postoji takav uslov onda se sracunava rename trace-a od blok-a
    if(b) {
        lhs1 = RenameExpressionVariables(fLBlocks[k]->GetTrace(), ff->GetContext(), ff->GetFunctionName());
        b = 0;
    }

//FIXME ovde dodati mehanizam inkrementalnog dodavanja blokova funkcije u dokazivac
      aExp lhs2 = RenameExpressionVariables(lc[m].LHS(), ff->GetContext(), ff->GetFunctionName());
      aExp lhs3 = aExp::AND(lhs1, lhs2);
      aExp rhs = RenameExpressionVariables(lc[m].RHS(), ff->GetContext(), ff->GetFunctionName());

      STATUS s = UNCHECKED;
      aExp lhs = aExp::AND(_State.Constraints(), lhs3);
//pitanje je da li ovaj konstruktor kopije radi kako treba
      LInstruction* ffi = new LInstruction(*(lc[m].Instruction()));
//      ffi->Stack().push_back(fi);
//      ffi->Contexts().push_back(ff->GetContext());
        ffi->AddIntoStack(fi, ff->GetContext());

      //ovo je pitanje da li je bolje ovako
      //ovde ima neki problem kada je povratna vrednost struktura
      //kod je takav da se ne vidi gde se ta memorija rezervise za strukturu
      //treba proveriti da li je u pitanju neka optimizacija koja to zezne
      if(Id() == 0)
      {
          aExp l = AddAddresses(aExp::AND(AllConstraints(), lhs3));
          s = LSolver::callSolver(l, rhs, this,ffi, lc[m].ErrorKind(), true);
          if(stopWhenFound(ffi, s, true) == -1)
            exit(1);
          if(FindFirstFlawed && Model && (s==UNSAFE || s==FLAWED))
              Delete(ffi->GetModelFileName());
      }

//FIXME proveriti da li se ovde mozda ne dodaju funkcijske adrese i zato nastane
//greska posle!!!!!!!!!!!!!!!!!!!!!!!
      LLocalCondition newlc(lhs, rhs, ffi, lc[m].ErrorKind(), s);
//      LLocalCondition newlc(aExp::AND(lhs, ff->GetFunctionConstraints()), rhs, ffi, lc[m].ErrorKind(), s);
      _LocalConditions.push_back(newlc);
      }
    }
  }
}


int LBlock::GetContext() const {
return _Parent->GetContext();
}



const LModule* LBlock::GetParentModule() const
{
    return GetParentFunction()->GetParentModule();
}


void LBlock::CalculateAssume()
{
bool b = false;
  for(unsigned i=0; i<_Instructions.size(); i++)
  {
    llvm::Instruction * instr = _Instructions[i]->Instruction();

    if((instr->getOpcode() == llvm::Instruction::Call) || (instr->getOpcode() == llvm::Instruction::Invoke))
    {
      unsigned numArgs;
      llvm::Function *f = GetFunction(instr, numArgs);

      if (f && f->isDeclaration())
      {
          if(f->getIntrinsicID() == llvm::Intrinsic::not_intrinsic)
            if(f->getName() == "assume" || f->getName() == "ASSUME")
                {b = true; break;}
      }
    }
  }
if(b) _HasAssume = true;
else _HasAssume = false;

}

//FIXME
//ovo bi verovatno moglo inteligentnije jer se ovako gubi mnogo informacija bez potrebe -
//trebalo bi obeleziti samo sta ucestvuje u assume i to prespajati, a ostalo nedirati
bool LBlock::HasAssume() const
{
return _HasAssume;
}

void LBlock::ChangeInitStore()
{
  if(!_PredsSet) SetPreds();
  if(_Preds.size() == 0) {_ChangeInitStore = true; return;}
  //ovo verovatno ne treba
  if(IsUnreachableBlock()) {_ChangeInitStore = true; return;}
  if(HasAssume()) {_ChangeInitStore = true; return;}

  for(unsigned i = 0; i<_Preds.size(); i++)
    if(!(_Preds[i]->StateIsSet())) _Preds[i]->SetState();


  std::vector<bool> change(_Preds.size());
  for(unsigned m=0; m<change.size(); m++)
      change[m] = false;

  std::string s;
  std::map<std::string, LVariableInfo>::const_iterator iter = _State.GetStore().GetStore().begin(), itere = _State.GetStore().GetStore().end();
  for(;iter!= itere; iter++)
  {
    s = iter->first;
    if(isAddress(iter->second.Value()->GetName()))
    { continue; }
    //ne znam zasto sam ovo dodala???
    if(isMalloc(iter->second.Value()->GetName()))
    { continue; }
    //ne znam zasto sam ovo dodala???
    if(isMalloc2(iter->second.Value()->GetName()))
    {continue;}

    aExp v1 = *_Preds[0]->_State.GetValue(s);
    bool b = false;
    for(unsigned j = 1; j<_Preds.size(); j++)
    {
      aExp v3 = *_Preds[j]->_State.GetValue(s);
      if(v1 != v3) {b = true; break;}
    }
    if(b) continue;

    _State.GetStore().ChangeValue(s, new aExp(v1));
    _State.GetStore().ChangeConnect(s, false);

    for(unsigned j = 0; j<_Preds.size(); j++)
    {
      if(_Preds[j]->_Jumps.size() == 1)
      {
        _Preds[j]->_State.GetStore().ChangeTransform(s,false);
        change[j]=true;
      }
      else
          //FIXME mozda dodati i opsti slucaj ako je jumps.size>2 ali ovo je zapetljano i bez toga
          if(_Preds[j]->_Jumps.size() == 2)
          {
            LBlock* fb;
            if(_Preds[j]->_Jumps[0].FB() != this) fb = _Preds[j]->_Jumps[0].FB();
            else fb = _Preds[j]->_Jumps[1].FB();
            if(fb->StateIsSet())
            //FIXME sta ako ovaj store nije postavljen? ako ga ovde postavimo - greska???
              if(!fb->_State.GetStore().GetConnect(s))
              {
                _Preds[j]->_State.GetStore().ChangeTransform(s,false);
                change[j]=true;
              }
          }
    }
  }

    for(unsigned j = 0; j<_Preds.size(); j++)
    if(change[j]) {_Preds[j]->RecalculatePostcondition();}

   _ChangeInitStore = true;
}



bool LBlock::IsEntryBlock() const
{
//FIXME uraditi smisleniju provery
if(_BBlock->hasName() && _BBlock->getName() == "entry")
  return true;

return false;
}



void LBlock::AddStore(const vStr& variables, const vVarInfo& fvis)
{

assert(variables.size() == fvis.size());
for(unsigned i=0; i<variables.size(); i++)
            _State.AddStore(variables[i], fvis[i]);

if(IsEntryBlock() && GetFunctionName() == StartFunction)
  {
    _State.AddGlobalInitializations();
    if(GetFunctionName() == "main") _State.AddMainArgumentConditions();
  }

}


const std::map<std::string, llvm::Type*>& LBlock::GetReferences() const {
return GetParentFunction()->GetReferences();
}


void LBlock::init()
{
    //Ovo vazi samo ako se blokovske lokalne promenljive ne koriste van bloka koji ih definise
    //  ResetGetOperandName();
    const InstructionInfoTable* infos = GetParentModule()->Infos();
    llvm::BasicBlock::iterator it = _BBlock->begin(), ie = _BBlock->end();
    for ( ; it != ie; ++it)
    {
      llvm::Instruction* i = it;
      const InstructionInfo* info = &infos->getInfo(i);
      _Instructions.push_back(new LInstruction(i, info, this));
    }

    CalculateAssume();
    _Addresses = aExp::TOP();

    std::map<llvm::BasicBlock*, vLoop >::iterator itloop = lav::FLoopBlocks.find(_BBlock);
    if(itloop!=lav::FLoopBlocks.end())
    {
    _Loop = itloop->second;
    lav::FLoopBlocks.erase(itloop);
    }

}


const std::string& LBlock::GetFunctionName   () const {
return _Parent->GetFunctionName();
}




int LBlock::stopWhenFound(const LInstruction* fi, STATUS s, bool count)
{
  if(!FindFirstFlawed) return 0;

  if(s==FLAWED || (count && (GetFunctionName() == StartFunction) && (s == UNSAFE)))
  {
    std::string sFilename=OutputFolder + "/" + ExtractFileName(InputFile) + ".html";

    const LModule* parent = NULL;

    if(PrintHtml)
    {

    const LModule* parent = GetParentModule();
    const LFunction* function = GetParentFunction();
    parent->PrintHTMLHeader(sFilename);
    function->PrintHTMLHeader(sFilename);
    }

    fi->PrintFlawedFoundResults(s);

    if(PrintHtml)
      parent->PrintHTMLFooter(sFilename);
    return -1;
  }
  else return 0;
}

const LBlock* LBlock::GetPredWithId(unsigned id) const
{
for(unsigned i=0; i< _Preds.size(); i++)
if(_Preds[i]->Id() == id) return _Preds[i];
return NULL;
}

void LBlock::ReCalculateConditions(int ex_size)
{
  if(_ConditionsCalculated) _ConditionsCalculated = false;
  if(IsUnreachableBlock()) {_ConditionsCalculated = true; return;}

  if(!_DescriptionsCalculated) CalculateDescriptions();
  if((_LocalConditions.size() == 0) && (_Jumps.size() == 0))
  {_ConditionsCalculated = true;  return;}
  if((_LocalConditions.size() == 0)) { _ConditionsCalculated = true; return; }

  //dodaje se uslovi ulaska u blok, ovo se dodaje samo temporary, bice dodato onda kada
  //ovaj blok postane nekome prethodnik
  aExp e = AddAddresses(BlockEntry());

  for(unsigned i=ex_size; i<_LocalConditions.size(); i++)
  {
    if((_LocalConditions[i].Status()==SAFE) || (_LocalConditions[i].Status()==FLAWED)
        || (_LocalConditions[i].Status()==UNREACHABLE))
    continue;

    aExp e1 = aExp::AND(e, _LocalConditions[i].LHS());
    aExp e2 = _LocalConditions[i].RHS();

    STATUS s = LSolver::instance().callSolverIncremental(e1, e2, this,
                                                        _LocalConditions[i].Instruction(),
                                                        _LocalConditions[i].ErrorKind());

    if(stopWhenFound( _LocalConditions[i].Instruction(), s,true) == -1) exit(1);
    if(FindFirstFlawed && Model && (s==UNSAFE || s==FLAWED))
      Delete(_LocalConditions[i].Instruction()->GetModelFileName());

    _LocalConditions[i].Status() = s;
  }
  _ConditionsCalculated = true;
}

bool LBlock::CheckReachability()
{
  if(_Reachable == BLOCK_REACHABLE) return true;  //reachable
  if(_Reachable == BLOCK_UNREACHABLE) return false; //unreachable

  aExp e1 = AddAddresses(GetTraceGlobFuncCons());
  STATUS s = LSolver::callSolver(e1);

  if(s==SAFE) _Reachable=BLOCK_UNREACHABLE; //unreachable
  else _Reachable=BLOCK_REACHABLE; //reachable

  return (_Reachable==BLOCK_REACHABLE);
}

void LBlock::CalculateConditions()
{

  if(_ConditionsCalculated) return;
  if(_LocalConditions.size() == 0) {_ConditionsCalculated = true; return;}

  aExp cond = AddAddresses(GetTraceGlobFuncCons());
  if(EnableParallel)
  {
    std::cout << "\n\n\n\n\n -----------------BRANISLAVA begin ------------------ \n\n\n\n\n";

    // napravi funkciju koju ce da izvrsava svaka nit
    auto maxf = [&](LLocalCondition *localCond, aExp *cond, LBlock *block, int i) {


            aExp e1 = aExp::AND(*cond, localCond->LHS());
            aExp e2 = localCond->RHS();
            std::cout << "\n\n\n\n\n -----------------Start solver , thread id: "<< pthread_self() <<" ------------------ \n\n\n\n\n" << std::endl;

            STATUS s = LSolver::callSolver(e1, e2, block,
                                              localCond->Instruction(),
                                              localCond->ErrorKind(), true);

            std::cout << "\n\n\n\n\n -----------------End solver , thread id: " <<  pthread_self() <<"------------------ \n\n\n\n\n" <<std::endl;


            //std::cout << FindFirstFlawed << " find first flawed" << std::endl;
            if(stopWhenFound(localCond->Instruction(), s, true) == -1)
            {
              return -1;
            }

            if(FindFirstFlawed && Model && (s == UNSAFE || s == FLAWED))
            {
              Delete(localCond->Instruction()->GetModelFileName());
              return -1;
            }

           localCond->Status() = s;
          return 0;
         };

    std::vector<std::function<int()>> functions;
    BindTasksForThreads.startTimer();
    for (unsigned i = 0; i<_LocalConditions.size(); i++)
    {

      if(SkipLocalCondition(_LocalConditions[i])) continue;

      // dodaj funkcije koje ce niti da izvrsavaju u red
      functions.push_back(std::bind(maxf,&_LocalConditions[i], &cond, this, i));
    }
    BindTasksForThreads.stopTimer();

    ParallelExecution.startTimer();
    // napravi thread pool i pokreni ga
    if(NumberThreads)
      ThreadPool t{FixedQueue<std::function<int()>>(functions) ,NumberThreads};
    else
      ThreadPool t{FixedQueue<std::function<int()>>(functions)};
    ParallelExecution.stopTimer();

    std::cout << "\n\n\n\n\n -----------------BRANISLAVA end ------------------ \n\n\n\n\n";
  }
  else
  {
    std::cout << "\n\n ----------------- BEGIN SEQUENTIAL LAV ------------------ \n\n";

    for(unsigned i=0; i<_LocalConditions.size(); i++)
    {
      if(SkipLocalCondition(_LocalConditions[i])) continue;

      aExp e1 = aExp::AND(cond, _LocalConditions[i].LHS());
      aExp e2 = _LocalConditions[i].RHS();
      STATUS s = LSolver::callSolver(e1, e2, this,
                                        _LocalConditions[i].Instruction(),
                                        _LocalConditions[i].ErrorKind(), true);

      if(stopWhenFound(_LocalConditions[i].Instruction(), s, true) == -1) exit(1);

      if(FindFirstFlawed && Model && (s==UNSAFE || s==FLAWED))
         Delete(_LocalConditions[i].Instruction()->GetModelFileName());

      _LocalConditions[i].Status() = s;
    }
       std::cout << "\n\n ----------------- END SEQUENTIAL LAV ------------------ \n\n";
  }

}

bool LBlock::IsInsideLoop() const
{
return (_Loop.size()!=0);
}


bool LBlock::DoCalculate() const
{
for(unsigned i=0; i<_Loop.size(); i++)
    if(_Loop[i].second >= 1) return false;
return true;
}

bool LBlock::DoNotCalculate() const
{

for(unsigned i=0; i<_Loop.size(); i++)
    if(_Loop[i].second >= 1 && _Loop[i].second < GetParentFunction()->GetLoopMax(_Loop[i].first))
      return true;

return false;
}

//vraca true ako je odradio posao, false ako ima jos nesto da se radi
bool LBlock::QuickCalculate()
{
  if(_ConditionsCalculated) return true;
  if(IsUnreachableBlock()) {_ConditionsCalculated = true; return true;}
  if(!_DescriptionsCalculated) CalculateDescriptions();
  if((_LocalConditions.size() == 0) && (_Jumps.size() == 0)) {_ConditionsCalculated = true;  return true;}

  //ako imamo prethodnika koji nije u dokazivacu dodamo ga
  AddPredsConditionsIncremental();

    if((_LocalConditions.size() == 0)) {
        _ConditionsCalculated = true;
        return true;
    }

    if(SkipInsideLoop && !(HasMerged()))
        if(IsInsideLoop() && DoNotCalculate()) {
            _LocalConditions.clear();
            _ConditionsCalculated = true;
            return true;
    }
  return false;
}


void LBlock::UpdateAddresses()
{
  unsigned curr = GetParentModule()->GetGlobalMemoryOffset();
  vaExp eqs;
  GetParentModule()->GetAddresses(eqs);
  _Parent->GetAddresses(curr, eqs);
  _Addresses = MakeANDFromExpressions(eqs);
}

void LBlock::UpdateAndSetAddresses()
{
  //moglo bi da se dodaju samo or-ovi da se nebi stvaralo djubre tipe 1=1 i 5=5 i slicno
  //adrese ne bi morale da se dodaju da je uvek jednakost, ali moze za malloc da bude or i onda
  //to mora da se doda jer to ne moze da se zameni
  UpdateAddresses();
  LSolver::instance().SetAddresses(_Addresses);
}


//ovaj metod treba da ide uklasu llocalcondition
bool LBlock::SkipLocalCondition(LLocalCondition& lc)
{
    if((lc.Status()==SAFE) || (lc.Status()==FLAWED) || (lc.Status()==UNREACHABLE))
{
        return true;
}

    if(SkipInsideLoop && (HasMerged()))
    {
        const LInstruction* fi = lc.Instruction();
        const LBlock* fb = fi->GetParentBlock();



        if(fb->IsInsideLoop() && fb->DoNotCalculate()) {
//ako se ne racuna trebalo bi ga izbaciti iz niza da se ne cuva izraz
            return true;
        }
    }

    const LInstruction* fi = lc.Instruction();
    if(SkipInsideLoop)
    {
        const LBlock* fb;
        if(!(fi->StackEmpty()))
            fi = fi->Stack()[fi->Stack().size()-1];
        fb = fi->GetParentBlock();
        if(fb->IsInsideLoop() && fb->DoNotCalculate())
        {
        return true;
        }
    }

 return false;
}

aExp LBlock::AllConstraints() const
{
/*    std::vector<aExp> exps;
    exps.push_back(_State.GetStateConstraint());
    exps.push_back(GetParentFunction()->GetFunctionConstraints());
    exps.push_back(GetParentModule()->GetGlobalConstraints());
    return MakeANDFromExpressions(exps);*/
    return aExp::AND(GetGlobFuncCons(), _State.GetStateConstraint());
}

aExp LBlock::AddAddresses(caExp& e)
{
/*    std::vector<aExp> exps;
    UpdateAndSetAddresses();
    exps.push_back(e);
    exps.push_back(_Addresses);
    return MakeANDFromExpressions(exps);*/
    UpdateAndSetAddresses();
    return aExp::AND(e, _Addresses);
}

aExp LBlock::GetGlobFuncCons() const
{
  return aExp::AND(GetParentFunction()->GetFunctionConstraints(), GetParentModule()->GetGlobalConstraints());
}
aExp LBlock::GetTraceGlobFuncCons()
{
/*    std::vector<aExp> exps;
    exps.push_back(GetParentFunction()->GetFunctionConstraints());
    exps.push_back(GetParentModule()->GetGlobalConstraints());
    exps.push_back(GetTrace());
    return MakeANDFromExpressions(exps);*/
    return aExp::AND(GetGlobFuncCons(), GetTrace());
}

caExp& LBlock::GetTrace()
{
if(_TraceCalculated) return _Trace;
_Trace = MakeTrace();
_TraceCalculated = true;
return _Trace;
}



aExp LBlock::MakeTrace() const
{
/*  std::vector<aExp> expressions;
  expressions.push_back(GetPredsConditions());
  expressions.push_back(GetEntryConditions());
  expressions.push_back(Active());
  return MakeANDFromExpressions(expressions);
  */
  return aExp::AND(GetPredsConditions(), BlockEntry());
}

aExp LBlock::Active() const
{ return ActiveBlock(GetFunctionName(), Id());}

aExp LBlock::BlockEntry() const
{
  return aExp::AND(GetEntryConditions(), Active());
}

void LBlock::CalculateConditionsBlock()
{
  if(QuickCalculate()) return;

  aExp F = AddAddresses(BlockEntry());
  std::vector< LLocalCondition* > conds;
  for(unsigned i=0; i<_LocalConditions.size(); i++)
  {
    //ovde bi mozda trebalo one koji se ne racunaju pobrisati da se ne cuvaju
    //i ne prave guzvu bezveze
    if(SkipLocalCondition(_LocalConditions[i]))
      {  continue; }
    conds.push_back(&_LocalConditions[i]);
  }

  STATUS s = LSolver::instance().callSolverBlock(F, conds, this);

  if(s!=SAFE && FindFirstFlawed)
      for(unsigned i=0; i<conds.size(); i++)
             if(stopWhenFound(conds[i]->Instruction(), s, true) == -1) exit(1);

  _ConditionsCalculated = true;
}

void LBlock::CalculateConditionsIncremental()
{
  if(CalculateBlock && _LocalConditions.size()>1)
  {
      CalculateConditionsBlock();
      return;
  }

  if(QuickCalculate()) return;

  aExp e = AddAddresses(BlockEntry());

  for(unsigned i=0; i<_LocalConditions.size(); i++)
  {
    if(SkipLocalCondition(_LocalConditions[i]))
        continue;

    aExp e1 = aExp::AND(e, _LocalConditions[i].LHS());
    aExp e2 = _LocalConditions[i].RHS();
//std::cout << "DODAJE SE ODAVDE!!!!!!!" << std::endl;
      STATUS s = LSolver::instance().callSolverIncremental(e1, e2, this, _LocalConditions[i].Instruction(), _LocalConditions[i].ErrorKind());

      if(stopWhenFound(_LocalConditions[i].Instruction(), s, true) == -1) exit(1);

    if(FindFirstFlawed && Model && (s==UNSAFE || s==FLAWED))
      Delete(_LocalConditions[i].Instruction()->GetModelFileName());

    _LocalConditions[i].Status() = s;
  }

  _ConditionsCalculated = true;

}

//izracunavaju se relevantne informacije za stor i uslovi za pojedine instrukcije
void LBlock::SetState()
{

if(!_ChangeInitStore)
{
//ChangeInitStoreTimer.startTimer();
ChangeInitStore();
//ChangeInitStoreTimer.stopTimer();
}
if(_StateIsSet) return;

//ovo je stvar koju state ne bi trebalo da proverava
if(IsUnreachableBlock())
{
  _StateIsSet = true;
  return;
}


//UpdateStoreTimer.startTimer();
  for(unsigned i=0; i<_Instructions.size(); i++)
      _State.Update(_Instructions[i]);
//UpdateStoreTimer.stopTimer();

//ako ima samo dva bloka u funkciji, onda model nije dobar jer se ovime izbace
//sve promenljive jer ni jedna nije relevantna
  _State.ExtractRelevant();
  _StateIsSet = true;
}

vUnsigned LBlock::GetLastLineStack() const
{
return _Instructions[_Instructions.size() - 1]->GetStackLines();
}

void LBlock::SetJumps()
{
  if(IsUnreachableBlock())
  {
    //FIXME ovde nije podeseno da ovaj blok nemoze da bude nikome sledbenik i to nedostaje!!!
    LFunction* f=GetParentFunction();

    //Svakom sledbeniku se postavlja da ovaj nije njegov predhodnik jer je unreachable
    llvm::succ_iterator it=llvm::succ_begin(_BBlock), ite = llvm::succ_end(_BBlock);
    for( ; it!=ite; it++)
    {
        llvm::BasicBlock* bb_succ=*it;
        LBlock* fb_succ = f->GetLBlock(bb_succ);
        if(fb_succ != NULL) fb_succ->AddNoPred(this);
    }
    return;
  }

  if(!_StateIsSet) SetState();

  LFunction* f=GetParentFunction();
  //postavljaju se FB u jump-ovima
  for(unsigned j=0; j<_Jumps.size(); j++)
  {
      LBlock* fb = f->GetLBlock(_Jumps[j].BB());
      assert(fb!=NULL);
      if(fb != NULL)
        _Jumps[j].SetFB(fb);
  }
}





void LBlock::SetPostconditionTop()
{
_Postcondition = aExp::TOP();
_PostconditionIsSet = true;
}

bool NotIn(const vpBlock& vfb, LBlock* fb)
{
for(unsigned i=0; i<vfb.size(); i++)
  if(vfb[i] == fb) return false;
return true;
}

void LBlock::SetPreds()
{
if(_PredsSet) return;
  LFunction* f=GetParentFunction();
  //Izracunavaju se svi predhodnici bloka - ovo ne moze u init-u
  llvm::pred_iterator it=llvm::pred_begin(_BBlock), ite = llvm::pred_end(_BBlock);
  for( ; it!=ite; it++)
  {
      llvm::BasicBlock* bb_pred=*it;
      LBlock* fb_pred = f->GetLBlock(bb_pred);
      if(fb_pred != NULL) fb_pred->CalculateDescriptions();
      else continue;
      if(fb_pred != NULL && (!fb_pred->HasNoReturnFunctionCall()) && NotIn(_NoPreds, fb_pred))
        _Preds.push_back(fb_pred);
  }
_PredsSet = true;

}

void LBlock::TryMerge()
{
  if(IsUnreachableBlock()) return;
  if(HasAssume()) return;

  if((_Preds.size() == 1) && (_Preds[0]->_Jumps.size() == 1)&& ((_Jumps.size() >= 1) || (_BBlock->hasName() && _BBlock->getName() == "return")))
  {

    LBlock* fb = _Preds[0];
    LBlockStore& st = _Preds[0]->_State.GetStore();

    //prethodniku izmeni store
    std::string s;
    std::map<std::string, LVariableInfo> addr;

    std::map<std::string, LVariableInfo>::const_iterator iter = st.GetStore().begin(), itere = st.GetStore().end();

    vStr vs;
    for(;iter!= itere; iter++)
    {
      if(iter->second.Connect()) vs.push_back(iter->first);
    }


    //ovo ne moze samo zbog connect-a
    _Preds[0]->_State.GetStore() = _State.GetStore();

    for(unsigned k=0; k<vs.size(); k++)
    {
      _Preds[0]->_State.GetStore().ChangeConnect(vs[k], true);
    }

    fb->_Jumps.clear();
    for(unsigned k = 0; k<_Jumps.size(); k++)
    {
      fb->_Jumps.push_back(_Jumps[k]);
      LBlock* jf = _Jumps[k].FB();
      jf->AddNoPred(this);
      jf->_Preds.push_back(fb);
    }

    aExp addconstr = fb->_State.Constraints();
    fb->_State.AddConstraint(_State.GetStateConstraint());
    for(unsigned k = 0; k<_LocalConditions.size(); k++)
    {
      _LocalConditions[k].LHS() = aExp::AND(addconstr, _LocalConditions[k].LHS());
      fb->_LocalConditions.push_back(_LocalConditions[k]);
    }


    if(HasNoReturnFunctionCall()) fb->SetNoReturnFunctionCall();
    //promeni mu postcond
    fb->RecalculatePostcondition();

    //ovo je dodato zbog paralelizacije, a ne treba ako nije paralelizacija
    //fb->ReCalculateConditions(ex_size);

    AddNoPred(fb);
    _Preds.clear();
    _Jumps.clear();
    _State.clear();
    _LocalConditions.clear();
    _Postcondition = aExp::TOP();
    _ConditionsCalculated = true;
    _MergeInfo.SetMerged(fb);
    fb->_MergeInfo.AddMerged(this);
    _ChangeInitStore = true;
    //fixme ovde petlje nismo oslobodili, jer nam ta informacija treba
    //za kasnije, to je bas lose resenje, ali sta da se radi, treba
    //smisliti nesto bolje
  }

}



void LBlock::AddToLoopMax() const
{
    if(!IsUnreachableBlock() && IsInsideLoop())
    {
    for(unsigned i=0; i<_Loop.size(); i++)
        GetParentFunction()->AddLoopMax(_Loop[i]);
    }
}

void LBlock::CalculateDescriptions()
{

  if(_DescriptionsCalculated) return;

  if(SkipInsideLoop)
    AddToLoopMax();

//std::cout << "SetPreds" << std::endl;
//SetPredsTimer.`start`Timer();
  SetPreds();
//SetPredsTimer.stopTimer();

//std::cout << "ChangeInitStore" << std::endl;
//ChangeInitStoreTimer.startTimer();
  ChangeInitStore();
//ChangeInitStoreTimer.stopTimer();

//std::cout << "SetState" << std::endl;
//SetStoreTimer.startTimer();
  SetState();
//SetStoreTimer.stopTimer();

//std::cout << "SetJumps" << std::endl;
//SetJumpsTimer.startTimer();
  SetJumps();
//SetJumpsTimer.stopTimer();

//std::cout << "SetPostcondition" << std::endl;
//SetPredsTimer.startTimer();
  SetPostcondition();
//SetPredsTimer.stopTimer();

//std::cout << "TryMerge()" << std::endl;
//TryMergeTimer.startTimer();
  TryMerge();
//TryMergeTimer.stopTimer();

//std::cout << "_DescriptionsCalculated" << std::endl;
  _DescriptionsCalculated = true;

}

void LBlock::RecalculatePostcondition()
{
_PostconditionIsSet = false;
SetPostcondition();
}



//id1 je tekuci id2 je prethodnik
//pid2_id1 => prespajanje id1_id2
aExp LBlock::ConnectBlocks(unsigned id1, unsigned id2) const
{
  aExp p = Transformation(GetFunctionName(), id2, id1);
  aExp e = _State.ConnectVariables(id1, id2);
  return aExp::IMPL(p,e);
}


  //dodaje se uslovi ulaska u blok, ovo se dodaje samo temporary,
  //bice stvarno dodato onda kada
  //ovaj blok postane nekome prethodnik
  //ovde je sada transformacija i uslovi ulaska
aExp LBlock::GetEntryConditions() const
{
  vaExp expressions;

  if(_Preds.size() == 0)
  {
        expressions.push_back(Active());
        return expressions[0];
  }

  if((_Preds.size() == 1) && HasAssume())
  {
      aExp e = ConnectBlocks(Id(), _Preds[0]->Id());
      expressions.push_back(e);
  }

  if(_Preds.size()>1)
  //uslovi prespajanja blokova
  for(unsigned j=0; j<_Preds.size(); j++)
      expressions.push_back(ConnectBlocks(Id(), _Preds[j]->Id()));

  vaExp exs;
  // svi prelasci u neki blok daju jedinstvenu promenljivu p13\/p23\/p03 <=> p3
  for(unsigned j=0; j<_Preds.size(); j++)
    exs.push_back(Transformation(GetFunctionName(), _Preds[j]->Id(), Id()));

  aExp e = aExp::TOP();
  if(_Preds.size() > 1) e = aExp::OR(exs);
  else if (_Preds.size() == 1) e = exs[0];

  expressions.push_back(aExp::IFF(e, Active()));

return MakeANDFromExpressions(expressions);
}

aExp LBlock::GetExitConditions() const
{
    vaExp expressions;

  if(_Jumps.size() == 1)
  {
    aExp pi = Active();
    aExp pij = Transformation(GetFunctionName(), Id(), _Jumps[0].FB()->Id());
    expressions.push_back(aExp::IFF(pi, pij));
  }

  if(_Jumps.size() > 1)
  {
    //pi /\cond_j <=> pij
    for(unsigned i=0; i<_Jumps.size(); i++)
    {
      aExp pi = Active();
      aExp pij = Transformation(GetFunctionName(), Id(), _Jumps[i].FB()->Id());
      expressions.push_back(aExp::IFF(aExp::AND(pi, _Jumps[i].Condition()), pij));
    }

    vaExp exprs;
    //pij \/ pik \/pil <=> pi
    for(unsigned i=0; i<_Jumps.size(); i++)
      exprs.push_back(Transformation(GetFunctionName(), Id(), _Jumps[i].FB()->Id()));
    expressions.push_back(aExp::IFF(aExp::OR(exprs), Active()));
  }

  return MakeANDFromExpressions(expressions);
}



aExp LBlock::GetPredsConditions() const
{
 vaExp expressions;
 vpBlock preds(_Preds);

 //pokupi sve uslove blokova svih prethodnika i prethodnika od prethodnika itd ali samo po jednom
 for(unsigned j=0; j<preds.size(); j++)
 {
    expressions.push_back(preds[j]->Postcondition());
    AddNewPreds(preds, preds[j]);
 }

 return MakeANDFromExpressions(expressions);
}

void LBlock::AddNewPreds(vpBlock& preds, LBlock* p) const
{
  bool b;
  for(unsigned k=0; k<p->_Preds.size(); k++)
  {
      b = true;
      for(unsigned l=0; l<preds.size(); l++)
           if(preds[l] == p->_Preds[k]) {b = false; break;}
      if(b) preds.push_back(p->_Preds[k]);
  }
}

void LBlock::AddPredsConditionsIncremental() const
{
   vpBlock preds(_Preds);

   //pokupi sve uslove blokova svih prethodnika i prethodnika od prethodnika itd ali samo po jednom
   for(unsigned j=0; j<preds.size(); j++)
   {
      if(preds[j]->PostconditionInSolver()) continue;
      if(!(preds[j]->PostconditionInSolver()))
      {
        //trebalo bi da su descriptions vec sracunati pa da ne moze da se mesa
        preds[j]->CalculateConditionsIncremental();
        //ovako moze da se doda neki postcondition pre nego sto mu se sracuna prethodnik i onda
        //to da bude problem --- a mozda i nije problem
        LSolver::instance().AddIntoSolver(preds[j]->Postcondition());
        preds[j]->PostconditionInSolver() = true;
      }
      AddNewPreds(preds, preds[j]);
   }
}


//Postavlja se postuslov bloka
//= Transformacija + uslovi izlaska iz bloka + odakle smo usli u blok
void LBlock::SetPostcondition()
{
//Ili ovde izmeniti ili u pravljenu postcondition od funckije izmeniti da se proverava da li je unreachable
if(_PostconditionIsSet) return;

if(IsUnreachableBlock())
{
  _Postcondition = aExp::TOP();
  _PostconditionIsSet = true;
  return;
}

//FIXME ovo ne moze ovde da stoji
  if(_NoJumps.size() > 0)
  {
    vJump p;

    for(unsigned i=0; i<_Jumps.size(); i++)
        if(NotIn(_NoJumps, _Jumps[i].FB()))
                p.push_back(_Jumps[i]);

    if(p.size()!=_Jumps.size())
    {
        _Jumps.clear();
        _Jumps = p;

        //odavde moze da se ode samo u blok za koji je zakljuceno da je unreachable pa je prema tome i ovaj unreachable, i to treba ponovo proslediti dalje
        if(_Jumps.size() == 0)
        {
            for(unsigned i=0; i<_Preds.size(); i++)
            _Preds[i]->AddNoJump(this);

            //postaviti da je ovaj blok unreachable i da se za njega vise nista ne racuna
            for(unsigned i=0; i<_Preds.size(); i++)
            {
            _Preds[i]->RecalculatePostcondition();
            _NoPreds.push_back(_Preds[i]);
            }
            _Preds.clear();
            //ovo bi trebalo da postavi sve da je unreachable
            //i da unazad postavi jos neki ako je unreachable
            RecalculatePostcondition();
        }
    }
  }


  vaExp expressions;
  expressions.push_back(_State.TransformationFormula());
  expressions.push_back(_State.GetStateConstraint());
  expressions.push_back(GetEntryConditions());
  expressions.push_back(GetExitConditions());

  assert(expressions.size()!=0);

  _Postcondition = MakeANDFromExpressions(expressions);
  _PostconditionIsSet = true;
}


void LBlock::AddLocalConditionPointers(caExp& r, LInstruction* fi)
{
  if(CheckPointers) AddLocalCondition(r, fi, BUFFEROVERFLOW);
}

bool LBlock::IsUnreachableBlock() const
{
return ((_Preds.size() == 0) && (_NoPreds.size()!=0)) || (_Reachable == BLOCK_UNREACHABLE);
}

void LBlock::FlawedFound(const LInstruction* fi, ERRKIND e)
{

bool reachable = CheckReachability();
if(reachable)
    {
      AddLocalConditionTimer.stopTimer();
      if(stopWhenFound(fi, FLAWED, false) == -1) exit(1);
       //ovo је dostizno ukoliko nije find-first-flawed
      _LocalConditions.push_back(LLocalCondition(aExp::TOP(),aExp::TOP(),fi,e, FLAWED));
      return;
    }
else
    {
      //Svakom sledbeniku se postavlja da ovaj nije njegov predhodnik jer je unreachable --- to bi trebalo da uradi setjumps

//FIXME saznanje da je ovaj unreachable, moze da utice da su jos neki unreachable i unapred i u nazad, i sada treba videti kako to zakrpiti, nije trivijalno, a nesto je vec i krpljeno
    //postaviti da on nije sledbenik njegovom prethodniku
    for(unsigned i=0; i<_Preds.size(); i++)
    _Preds[i]->AddNoJump(this);

    //postaviti da je ovaj blok unreachable i da se za njega vise nista ne racuna
    for(unsigned i=0; i<_Preds.size(); i++)
    {
//ovde bi mozda trebalo i ponovo sracunati uslove --- npr ako je upetlji preskoceno zato sto je zakljuceno da ne treba jer je u petlji, sada kada se izmeni ovaj da nije poslednji jer je unreachable, onda mozda treba sracunati u nekom prethodnom
    _Preds[i]->RecalculatePostcondition();
    _NoPreds.push_back(_Preds[i]);
    }

    _Preds.clear();
    _LocalConditions.clear();
     RecalculatePostcondition();
     AddLocalConditionTimer.stopTimer();
    return;
    }
}

bool LBlock::ProcessStatus(const LInstruction* fi, ERRKIND e, STATUS s)
{
    if(s == FLAWED)
    {
      FlawedFound(fi,e);
      return true;
    }

    if(s == SAFE)
    {
      aExp t = aExp::TOP();
      _LocalConditions.push_back(LLocalCondition(t,t,fi,e, s));
      AddLocalConditionTimer.stopTimer();
      return true;
    }

    return false;
}


//ovo bi sve moglo da se pojednostavi
void LBlock::AddLocalCondition(caExp& r, LInstruction* fi, ERRKIND e)
{
    STATUS s;
    AddLocalConditionTimer.startTimer();

    if(IsUnreachableBlock())
    {
//        _LocalConditions.push_back(LLocalCondition(aExp::TOP(),aExp::TOP(),fi, e, UNREACHABLE));
        AddLocalConditionTimer.stopTimer();
        return;
    }

    if(SkipInsideLoop)
    {
      //prvi ulazak u petlju ce ipak da proveri, a poslednji nece jer jos
      //uvek ne zna koji je poslednji
      if(IsInsideLoop() && !DoCalculate())
      {
        //bilo bi dobro ovo nekako ne dodavati jer se jednako nece proveravati
        _LocalConditions.push_back(LLocalCondition(_State.Constraints(),r,fi, e, UNCHECKED));

        AddLocalConditionTimer.stopTimer();
      return;
      }
    }

    aExp t = aExp::TOP();
    s = LSolver::callSolver(t, r, this, fi, e);
    if(ProcessStatus(fi,e,s) == true) return;

    aExp l = AddAddresses(AllConstraints());
    s = LSolver::callSolver(l, r, this,fi, e, true);

    if(Id() == 0)  if(stopWhenFound( fi, s, true) == -1) exit(1);
    if(ProcessStatus(fi,e,s) == true) return;

    //ako je u prvom bloku, onda nema potrebe ponovo da se racuna u odnosu na prethodne blokove,
    //ako je u nekom narednom bloku, onda stavljamo da je unchecked
    if(Id()!=0) s = UNCHECKED;
    _LocalConditions.push_back(LLocalCondition(_State.Constraints(),r,fi,e, s));
    AddLocalConditionTimer.stopTimer();
}


void LBlock::AddLocalConditionZeroDisequality(caExp& e, LInstruction* fi)
{
    if(CheckDivZero)
    {
        const llvm::Type* t = (fi->Instruction())->getType();
        aExp r = aExp::Disequality(e, ExpNumZero(GetIntType(t)));
        AddLocalCondition(r, fi, DIVISIONBYZERO);
    }
}

//ovo je cudno, i ne znam cemu sluzi, dodato je zbog ssphi ali ne znam zasto
LBlock* LBlock::GetPred() const
{
	    LBlock* pred = _Preds[0];
	    if(pred->HasMerged()){
        for(unsigned k=0; k<pred->_MergeInfo.NumMerged(); k++)
            pred =  pred->_MergeInfo.LastMerged();
        }
      return pred;
}


} //end of namespace
