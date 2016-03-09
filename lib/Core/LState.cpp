//===-- LState.cpp --------------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
 
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

#include "config.h"
#include "lav/Internal/LState.h"
#include "lav/Internal/LCommon.h"
#include "lav/Internal/LInstruction.h"
#include "lav/Internal/LBlock.h"
#include "lav/Internal/LModule.h"
#include "lav/Internal/LFunction.h"
#include "lav/Internal/LExpressionTransformation.h"
#include "lav/Internal/LTypes.h"

#include "expression/output/SMTFormater.h"
#include "expression/expressions/NumeralTypes.h"

#include <vector>

extern llvm::cl::opt<bool> Model;
extern llvm::cl::opt<bool> Students;
extern llvm::cl::opt<bool> TrackPointers;
extern llvm::cl::opt<bool> SkipInsideLoop;
extern llvm::cl::opt<bool> MemoryFlag;
extern llvm::cl::opt<bool> CheckAssert;

namespace lav
{

  llvm::cl::opt<bool>
  urm("urm",
                       llvm::cl::desc("LAV --- urm (default=false)"),
                       llvm::cl::init(false));

  llvm::Timer Branching("Branching time --- call solver light");


static argo::SMTFormater SMTF;

void LState::WriteIntoStore(llvm::Instruction* i, const aExp& e)
{
    if(e.isSelect() && (GetIntType(i->getType())!=fint_type))
      _Store.Write(GetOperandName(i), new aExp(SetWidth(e, fint_type, GetIntType(i->getType()))), i->getType());
    else
      //podrazumeva se da je relevant = false
      _Store.Write(GetOperandName(i), new aExp(e), i->getType());
}


void LState::Write(const std::string &name, const LVariableInfo& info)
{
_Store.Write(name, info);
}       

void LState::Write(const std::string &name, aExp* value, const llvm::Type* type, bool relevant)
{
_Store.Write(name, value, type, relevant);
}

//Pretpostavka - argument je ili varijabla ili konstanta
aExp LState::argument(const llvm::Instruction* i, unsigned int n)
{
  assert(i->getNumOperands() > n);
  if(llvm::Constant *c = llvm::dyn_cast<llvm::Constant>(i->getOperand(n))) 
{
return evalConstant(c);
}
//ovde u principu nije bitan ni tip ni relevant jer se koristi samo za _store.getvalue
  else 
{
return ExpVar(GetOperandName(i->getOperand(n)), GetIntType(i->getType()), false);
}
}

void LState::ProcessStore(LInstruction* fi)
{
  llvm::Instruction* i = fi->Instruction();
  assert(i->getNumOperands() != 2);

//llvm::outs () << *i <<'\n';
//std::cout<<'\n'<< "pre store--------------------------" <<'\n';
//_Store.Print(std::cout);
//std::cout<< "pre store--------------------------" <<std::endl;
//PrintConstraints();


  //store sta gde - ako je gde gepi 
  llvm::GetElementPtrInst *gepi = llvm::dyn_cast<llvm::GetElementPtrInst>(i->getOperand(1));  
  if(gepi)
  {
    aExp arg1 = argument(i, 1);
    GepiArgument(arg1, fi);
    aExp index = GetValue(arg1);
    if(!TrackPointers) return;
    MemoryStore(fi, index);
  }
  //dakle to gde upisujemo tj prvi operand nije gepi
  else 
  //sta ako je u pitanju *p
//FIXME ovo je dubiozno
  if((i->getOperand(1)->getType() == _Store.GetType(GetOperandName(i->getOperand(1))))
    ||   
    (  llvm::dyn_cast<llvm::Instruction>(i->getOperand(1)) && (llvm::dyn_cast<llvm::Instruction>(i->getOperand(1))->getOpcode() == llvm::Instruction::Load)) )

  {
    aExp arg1 = argument(i, 1);
    aExp index = GetValue(arg1);
    if(TrackPointers) MemoryStore(fi, index);
    llvm::Type* t = i->getOperand(0)->getType();
    unsigned size = GetParentBlock()->GetParentModule()->GetTargetData()->getTypeAllocSize(t);
    aExp r1 = aExp::sle(ExpNum1(size, fint_type), ExpRight(index));
    aExp r2 = aExp::sge(ExpNumZeroInt, ExpLeft(index));
    aExp r = aExp::AND(r1,r2);
    GetParentBlock()->AddLocalConditionPointers(r, fi);
  }
  //nije ni gepi ni *p, da li je globalna promenljiva?
  else    
  if(llvm::dyn_cast<llvm::GlobalValue>(i->getOperand(1)))
  {
    //ovo deluje suludo, kao da je dovoljno samo
      aExp value  = StoreValue(fi);
      StoreGlobalValue(GetOperandName(i->getOperand(1)), value, i);
  }
  else
  //ali je mozda konstantan gepi
  if(llvm::Constant *c = llvm::dyn_cast<llvm::Constant>(i->getOperand(1)))
  {
    aExp arg1 = argument(i, 1);
    aExp base = ExpGlobalAddress(GetOperandName(c->getOperand(0)));
    aExp addend = arg1[1];

    aExp r1 = aExp::slt(addend, ExpRight(base));
    aExp r2 = aExp::sge(addend, ExpLeft(base));
    aExp r = aExp::AND(r1,r2);
    GetParentBlock()->AddLocalConditionPointers(r, fi);

    if(!TrackPointers) return;
    aExp index = SimplifyExpression(aExp::add(base, addend));
    MemoryStore(fi, index);
  }
  else
  {
    aExp arg0 = argument(i, 0);
    std::string name = GetOperandName(i->getOperand(1));

    if(_Store.IsDefined(name))
{
//if(!(fi->Instruction()->getOpcode() == llvm::Instruction::Store))

//fixme ovo je jeziva zakrpa uzeto iz StoreValue sa copy paste
  aExp arg0 = argument(i, 0);
  aExp argRes;
  llvm::GetElementPtrInst *gepi = llvm::dyn_cast<llvm::GetElementPtrInst>(i->getOperand(0));

  if(llvm::dyn_cast<llvm::GlobalValue>(i->getOperand(0)))
    //ovo je sumnjivo    
    argRes = ExpGlobalAddress(GetOperandName(i->getOperand(0)));
  else
  if(gepi)
  {
//    GepiArgument(arg0, fi);
    argRes = GetValue(arg0);
  }
  //ovo je sumnjivo
  //ako je u pitanju citanje adrese promenljive b onda vracamo amp_b
  else if(arg0.IsVariable() && !_Store.IsDefined(arg0.GetName()))
    {  argRes = ExpAddress(GetParentBlock()->GetFunctionName(), arg0.GetName());}
  else
        {
          argRes = GetValue(arg0);
        }

//      _Store.ChangeValue(name, new aExp(StoreValue(fi)));
      _Store.ChangeValue(name, new aExp(argRes));
}    else 
    {
    if(!TrackPointers) return;

    aExp index = ExpAddress(GetParentBlock()->GetFunctionName(), name);
    MemoryStore(fi, index);
    }
  }
}


static bool isConstantAllOnes(llvm::Value* V)
{
  if (const llvm::ConstantInt *CI = llvm::dyn_cast<llvm::ConstantInt>(V))
    return CI->isAllOnesValue();
  if (const llvm::ConstantVector *CV = llvm::dyn_cast<llvm::ConstantVector>(V))
    return CV->isAllOnesValue();
  return false;
}

void LState::ProcessXor(LInstruction* fi)
{  
  aExp e;
  llvm::Instruction* i = fi->Instruction();

  //not se implementira preko xor- ako je 
  //xor A true 
  //onda je to zapravo 
  //not A
  if(isConstantAllOnes(i->getOperand(0)) || isConstantAllOnes(i->getOperand(1)))
  {
    aExp ee;
    if(isConstantAllOnes(i->getOperand(0)))
        ee = *GetValue(GetOperandName(i->getOperand(1)));
    else
        ee = *GetValue(GetOperandName(i->getOperand(0)));
    e = aExp::NOT(ee);
  }
  else 
    e = aExp::bitXor(GetValue(argument(i,0)), GetValue(argument(i,1)));
  
  WriteIntoStore(i,e);
}

void LState::ProcessFChange(LInstruction* fi, llvm::Function* f, unsigned numArgs)
{
//if(urm) return;

    llvm::Instruction* i = fi->Instruction();
    for(unsigned j=0; j<numArgs; ++j)
    {
//      const llvm::Type * t = i->getOperand(argStart+j)->getType();
      std::string name = GetOperandName(i->getOperand(j));
      
      //nije GetIntType(i->getOperand(argStart+j)->getType()) 
      //jer to moze da se razlikuje, jer je tip pokazivac 
      //a mi nekada koristimo ono nasta on pokazuje, 
      //zato je jedino bezbedno da se uzme iz stora tip
      aExp e1 = ExpVar(GetNameOfNextVariable(), GetIntType(_Store.GetType(name)), false );

      _Store.ChangeValue(name, new aExp(e1));

    }
}

void LState::ProcessFEnd(LInstruction* fi, llvm::Function* f, unsigned numArgs)
{
      aExp e = aExp::NOT(GetParentBlock()->Active());
//      aExp cond = aExp::NOT(aExp::Equality(ExpNumZeroInt,ExpNumZeroInt));
      aExp cond = ExpPropVar;
      if(urm) { 
        GetParentBlock()->AddLocalCondition(cond,fi, OTHER); 
//        _Constraints.Add(aExp::Equality(ExpNumZeroInt, ExpNumZeroInt));
      } 
      _Constraints.Add(e);
}

void LState::ProcessFInc(LInstruction* fi, llvm::Function* f, unsigned numArgs)
{
    llvm::Instruction* i = fi->Instruction();
    if(numArgs == 0) return;
    //fixme tip
    aExp k = ExpVar(GetNameOfNextVariable(), GetIntType(i->getOperand(0)->getType()), false );

      for(unsigned j=0; j<numArgs; ++j)
    {
      const llvm::Type * t = i->getOperand(j)->getType();
      std::string name = GetOperandName(i->getOperand(j));
      aExp e1 = ExpVar(GetNameOfNextVariable(), GetIntType(t), false );
       const aExp* curr_value = GetValue(name);
      _Constraints.Add(aExp::sge(e1, *curr_value));
      if(urm)
          _Constraints.Add(aExp::Equality(e1, aExp::add(*curr_value, k)));
      _Store.ChangeValue(name, new aExp(e1));

    }


}

void LState::ProcessFDec(LInstruction* fi, llvm::Function* f, unsigned numArgs)
{
    llvm::Instruction* i = fi->Instruction();
    for(unsigned j=0; j<numArgs; ++j)
    {
      const llvm::Type * t = i->getOperand(j)->getType();
      std::string name = GetOperandName(i->getOperand(j));
      aExp e1 = ExpVar(GetNameOfNextVariable(), GetIntType(t), false );
       const aExp* curr_value = GetValue(name);
      _Constraints.Add(aExp::sle(e1, *curr_value));
      _Store.ChangeValue(name, new aExp(e1));
    }
}

void LState::ProcessSelect(LInstruction* fi)
{  
    llvm::Instruction* i = fi->Instruction();
    aExp cond = GetValue(argument(i, 0));
    aExp then = GetValue(argument(i, 1));
    aExp elsee = GetValue(argument(i, 2));
    aExp e = aExp::IfThenElse(cond, then, elsee);
    WriteIntoStore(i, e);

}

//mozda bi trebalo reorganizovati store, da ima obican i da ima globalni deo
//pa onda ovo lepo sve u store prebaciti
void LState::StoreGlobalValue(const std::string& name, aExp& value, llvm::Instruction* i) 
{
std::string s = GlobalName(name);
if(_Store.IsDefined(s))
    _Store.ChangeValue(s, new aExp(value));
else
    {
    if(!TrackPointers) return;
    aExp mem = *GetValue(MEMORY);
    aExp index = ExpGlobalAddress(name);

    if(GetIntType(i->getOperand(0)->getType())!=fint_type) 
        value = SetWidth(value, GetIntType(i->getOperand(0)->getType()),fint_type);

    MemoryStore(mem, index, value);
    }
}

void LState::ProcessICmp(LInstruction* fi)
{
    llvm::Instruction* i = fi->Instruction();
    llvm::CmpInst *ci = llvm::cast<llvm::CmpInst>(i);
    llvm::ICmpInst *ii = llvm::cast<llvm::ICmpInst>(ci);

    aExp e0 = GetValue(argument(i,0));
    aExp e1 = GetValue(argument(i,1));

    switch(ii->getPredicate()) {
    case llvm::ICmpInst::ICMP_EQ: {
    aExp e = aExp::Equality(e0, e1);
    WriteIntoStore(i,e);
      break;
    }

    case llvm::ICmpInst::ICMP_NE: {
    aExp e = aExp::Disequality(e0, e1);
    WriteIntoStore(i,e);
      break;
    }

    case llvm::ICmpInst::ICMP_UGT: {
    aExp e = aExp::ugt(e0, e1);
    WriteIntoStore(i,e);
      break;
    }

    case llvm::ICmpInst::ICMP_UGE: {
    aExp e = aExp::uge(e0, e1);
    WriteIntoStore(i,e);
      break;
    }

    case llvm::ICmpInst::ICMP_ULT: {
    aExp e = aExp::ult(e0, e1);
    WriteIntoStore(i,e);
      break;
    }

    case llvm::ICmpInst::ICMP_ULE: {
    aExp e = aExp::ule(e0, e1);
    WriteIntoStore(i,e);
      break;
    }

    case llvm::ICmpInst::ICMP_SGT: {
    aExp e = aExp::sgt(e0, e1);
    WriteIntoStore(i,e);
      break;
    }

    case llvm::ICmpInst::ICMP_SGE: {
    aExp e = aExp::sge(e0, e1);
    WriteIntoStore(i,e);
      break;
    }

    case llvm::ICmpInst::ICMP_SLT: {
    aExp e = aExp::slt(e0, e1);
    WriteIntoStore(i,e);
      break;
    }

    case llvm::ICmpInst::ICMP_SLE: {
    aExp e = aExp::sle(e0, e1);
    WriteIntoStore(i,e);
      break;
    }

    default: {
        assert("unexpected command!");
        std::cerr<< "unexpected command!"<< std::endl;
        exit(1);
  }
}

}

void LState::ProcessFCmp(LInstruction* fi)
{
  llvm::Instruction* i = fi->Instruction();
    llvm::CmpInst *ci = llvm::cast<llvm::CmpInst>(i);
    llvm::FCmpInst *ii = llvm::cast<llvm::FCmpInst>(ci);


    aExp e0 = GetValue(argument(i,0));
    aExp e1 = GetValue(argument(i,1));

    switch(ii->getPredicate()) {
    case llvm::FCmpInst::FCMP_OEQ: 
    case llvm::FCmpInst::FCMP_UEQ: 
	{
    aExp e = aExp::Equality(e0, e1);
    WriteIntoStore(i,e);
      break;
    }

    case llvm::FCmpInst::FCMP_ONE: 
    case llvm::FCmpInst::FCMP_UNE: 
	{
    aExp e = aExp::Disequality(e0, e1);
    WriteIntoStore(i,e);
      break;
    }

    case llvm::FCmpInst::FCMP_OGT: 
    case llvm::FCmpInst::FCMP_UGT: 
	{
    aExp e = aExp::ugt(e0, e1);
    WriteIntoStore(i,e);
      break;
    }

    case llvm::FCmpInst::FCMP_OGE:
    case llvm::FCmpInst::FCMP_UGE: {
    aExp e = aExp::sge(e0, e1);
    WriteIntoStore(i,e);
      break;
    }

    case llvm::FCmpInst::FCMP_OLT:
    case llvm::FCmpInst::FCMP_ULT: {
    aExp e = aExp::slt(e0, e1);
    WriteIntoStore(i,e);
      break;
    }

    case llvm::FCmpInst::FCMP_OLE:
    case llvm::FCmpInst::FCMP_ULE: {
    aExp e = aExp::sle(e0, e1);
    WriteIntoStore(i,e);
      break;
    }

    default: {
        assert("unexpected command!");
        std::cerr<< "unexpected command!"<< std::endl;
        exit(1);
  }
}

}
//izmenjeno allocationinst
void LState::WriteAllocation(LConstraints& constraints, llvm::AllocaInst *ai, unsigned left, unsigned right)
{
//    const llvm::Type* t = ai->getAllocatedType();
    std::string name = GetOperandName(ai);
    aExp e;
  if(_Store.IsDefined(name)) 
    e = *GetValue(name);
  else
  {//FIXME cudno da ovde ne treba neka fiksna adresa
    argo::IntType iType = GetIntType(ai->getAllocatedType());
    //    IntType iType = GetIntType(ai->getType());
    //FIXME tip treba da bude pointer type
    e = ExpVar(GetNameOfNextVariable(), iType, false);
    //    WriteIntoStore(ai, e);
    _Store.Write(GetOperandName(ai), new aExp(e), ai->getAllocatedType());
  }

    constraints.AddConstraint(e, left, right);
}

//FIXME postaviti i druge varijante za alokaciju, 
// sve za malloc i za alloca npr za alokaciju struktura i slicno
void LState::ProcessAllocation(LInstruction* fi)
{
  llvm::Instruction* i = fi->Instruction();

//fixmeT izmenjeno allocationinst
  llvm::AllocaInst *ai = llvm::dyn_cast<llvm::AllocaInst>(i);
  const llvm::Type* t = ai->getAllocatedType();

  if(llvm::dyn_cast<llvm::PointerType>(t))
  { 
    //Ako je alociran pointer njegovi left i right se postavljaju na nulu
    WriteAllocation(_Constraints, ai, 0, 0);
  }
  else
  {
    //ako je npr niz granice u bajtovima su postavljne u funkciji
    //alokacija niza ili strukture
    if(t->isSized() && t->isAggregateType())
    {
        //assert(ai->hasName());
        if(!_Store.IsDefined(GetOperandName(ai))) 
        {
          aExp e = ExpAddress(GetParentBlock()->GetFunctionName(), GetOperandName(ai));
         _Store.Write(GetOperandName(ai), new aExp(e), ai->getAllocatedType());
        }
    }
    //ako je obican int onda ne postavljamo nista
    else 
    {
      //ovo je na primer za alokaciju int-a 
      if(_Store.IsDefined(GetOperandName(ai))) 
          return;
      //FIXME ovo treba i za pointere
      //ako se negde uzima referenca od ovog int-a onda se ne ubacuje u store
      if(GetParentBlock()->GetParentFunction()->GetReferences().find(GetOperandName(ai))!=GetParentBlock()->GetParentFunction()->GetReferences().end())
      {
        return;
      }
      argo::IntType iType = GetIntType(ai->getAllocatedType());
//      IntType iType = GetIntType(ai->getType());

      std::string name = GetOperandName(ai) + "_" + GetNameOfNextVariable();
      aExp e = ExpVar(name, iType, false);

//      WriteIntoStore(ai, e); //ovo je sa gettype pa ne moze
    _Store.Write(GetOperandName(ai), new aExp(e), ai->getAllocatedType());
      }
  }
}

void LState::ProcessPtrToInt(LInstruction* fi)
{
  llvm::Instruction* i = fi->Instruction();
  aExp ee = *GetValue(GetOperandName(i->getOperand(0)));
  aExp e = PtrToInt(ee);
  WriteIntoStore(i, e);
}

void LState::ProcessIntToPtr(LInstruction* fi)
{
  llvm::Instruction* i = fi->Instruction();
  aExp ee = *GetValue(GetOperandName(i->getOperand(0)));
  aExp e = IntToPtr(ee);
  WriteIntoStore(i, e);
}
//FIXME Ovo nesto nije u redu kada se npr doda assert(right(a) == 40) to dodaje bitcast i kao da se ne prepisu left i right i onda ima manjak informacija
void LState::ProcessBitCast(LInstruction* fi)
{
 
  llvm::Instruction* i = fi->Instruction();
  if(i->getName() != "alloca point")
  {



  const llvm::Type* t = i->getType();
  unsigned width_t = GetBitWidth(GetIntType(t));
  unsigned width_s = GetBitWidth(GetIntType(_Store.GetType(GetOperandName(i->getOperand(0)))));
  aExp e1 = *GetValue(GetOperandName(i->getOperand(0)));

//  std::cout << "BITCAST ovde sam" << std::endl;
//  e1.Print(&SMTF, std::cout);
//  std::cout << std::endl;

  //FIXME ovo proveriti
  if(width_s == argo::noInt) width_s = fint_type;
  aExp e = SetWidth(e1, width_s, width_t); 
  WriteIntoStore(i, e);
  }
}

void LState::GepiArgument(const aExp& arg, LInstruction* fi)
{
    aExp e = GetValue(arg);
    aExp base;
    aExp addend;
    if((e.GetArity() == 0)||(e.GetArity() == 1) || e.isSelect()) {
    base = e; addend = ExpNumZeroInt;}
    else if(e.GetArity() == 2)
    {
    base = e[0];
    addend = e[1];

    if(GetBitWidth(fint_type) != GetBitWidth(fpointer_type))
      addend = e[1][0];
    else 
      addend = e[1];
    }
    aExp r1 = aExp::slt(addend, ExpRight(base));
    aExp r2 = aExp::sge(addend, ExpLeft(base));
    aExp r = aExp::AND(r1,r2);
    GetParentBlock()->AddLocalConditionPointers(r, fi);
}



void LState::ProcessLoad(LInstruction* fi)
{
  llvm::Instruction* i = fi->Instruction();
  assert(i->getNumOperands() != 1);
  aExp e;

  llvm::Instruction* ii = llvm::dyn_cast<llvm::Instruction>(i->getOperand(0));
  llvm::GetElementPtrInst *gepi = llvm::dyn_cast<llvm::GetElementPtrInst>(i->getOperand(0));

  if(gepi)
  {
    aExp arg0 = argument(i, 0);
    GepiArgument(arg0, fi);
//fixme tip
    if(!TrackPointers) e = ExpVar(GetNameOfNextVariable(), argo::sInt32Type, false);
    else
    {
    aExp ee = GetValue(arg0);
    aExp mem = *GetValue(MEMORY);
    e = aExp::select(mem,ee);
    }
  }
  else
  //ako je load *p
//  if(!i->getOperand(0)->hasName()) ---
  if((ii && ii->getOpcode() == llvm::Instruction::Load)) 
  {
    aExp arg0 = argument(i, 0);
    aExp index = GetValue(arg0);
    llvm::Type* t = i->getOperand(0)->getType();
    unsigned size = GetParentBlock()->GetParentModule()->GetTargetData()->getTypeAllocSize(t);
    aExp r1 = aExp::sle(ExpNum1(size, fint_type), ExpRight(index));
    aExp r2 = aExp::sge(ExpNumZeroInt, ExpLeft(index));
    aExp r = aExp::AND(r1,r2);
    GetParentBlock()->AddLocalConditionPointers(r, fi);

    if(!TrackPointers) 
        //fixme tip
        e = ExpVar(GetNameOfNextVariable(), argo::sInt32Type, false);
    else
    {
    aExp mem = *GetValue(MEMORY);
    e = aExp::select(mem, index);
    }
  }
  else    
  if(llvm::dyn_cast<llvm::GlobalValue>(i->getOperand(0)))
  {
      e = GetGlobalValue(GetOperandName(i->getOperand(0)));
  }
  else
  //ali je mozda konstantan gepi
  //FIXME ovo je sumnjivo
  if(llvm::Constant* c = llvm::dyn_cast<llvm::Constant>(i->getOperand(0)))
  {
    aExp arg0 = argument(i, 0);
    //FIXME ?!? zasto global address
    aExp base = ExpGlobalAddress(GetOperandName(c->getOperand(0)));
    aExp addend = arg0[1];
    aExp r1 = aExp::slt(addend, ExpRight(base));
    aExp r2 = aExp::sge(addend, ExpLeft(base));
    aExp r = aExp::AND(r1,r2);
    GetParentBlock()->AddLocalConditionPointers(r, fi);


    if(!TrackPointers)    
        //fixme tip 
        e = ExpVar(GetNameOfNextVariable(), argo::sInt32Type, false);
    else
    {
    aExp index = SimplifyExpression(aExp::add(base, addend));
    aExp mem = *GetValue(MEMORY);
    e = aExp::select(mem,index);
    }
  }
  else
  {
    aExp arg0 = argument(i, 0);
    e = GetValue(arg0);
  }

  WriteIntoStore(i, e);
}


const aExp& LState::GetValue(const aExp& e) 
{
    if(e.IsVariable())
            {return *GetValue(e.GetName());}
    return e;
}

const aExp* LState::GetValue(const std::string &name) 
{

    const aExp* e = _Store.GetValue(name);
    if(e!=NULL) return e;
    else 
    {
      aExp e = aExp::TOP();
      if((GetParentBlock()->GetReferences().find(name)) != (GetParentBlock()->GetReferences().end()))  
      { 

//std::cout << "nije pronadjena "<<name << std::endl;

        if(TrackPointers)
        {
           e = aExp::select(*GetValue(MEMORY), ExpAddress(GetParentBlock()->GetFunctionName(), name));
        }
        else 
        {
        //fixme tip
         e = ExpVar(GetNameOfNextVariable(), argo::sInt32Type, false);
        }
      }
      else //trebalo bi da je onda globalna
      {
         e = ExpGlobalAddress(name);
      }
      //pitanje je da li ovo treba
      //for(unsigned i=0; i<_Memory.size(); i++)
      //  if(_Memory[i]==e) return (&(_Memory[i]));
      _Memory.push_back(e);
      return (&(_Memory[_Memory.size() - 1]));

    }
      
}



aExp LState::StoreValue(LInstruction* fi)
{
  llvm::Instruction* i = fi->Instruction();
  assert(i->getNumOperands() != 2);
  aExp arg0 = argument(i, 0);

  llvm::GetElementPtrInst *gepi = llvm::dyn_cast<llvm::GetElementPtrInst>(i->getOperand(0));

  if(llvm::dyn_cast<llvm::GlobalValue>(i->getOperand(0)))
    //ovo je sumnjivo    
    return ExpGlobalAddress(GetOperandName(i->getOperand(0)));
  else
  if(gepi)
  {
    GepiArgument(arg0, fi);
    return GetValue(arg0);
  }
  //ovo je sumnjivo
  //ako je u pitanju citanje adrese promenljive b onda vracamo amp_b
  else if(arg0.IsVariable() && !_Store.IsDefined(arg0.GetName()))
    {  
//std::cout << "mozda ovde " <<GetParentBlock()->GetFunctionName() << "    " << arg0.GetName() << std::endl;
return ExpAddress(GetParentBlock()->GetFunctionName(), arg0.GetName());}
  else
        {
          return GetValue(arg0);
        }

}

void LState::MemoryStore(const aExp& mem, const aExp& index, const aExp& value)
{
    if(!TrackPointers) return;
    aExp store =  aExp::store(mem, index, value);


//OVO JE DA SE PRATI JEDAN VELIKI NIZ STOROVA UMESTO UBACIVANJA NOVE PROMENLJJIVE ZA VREDNOST MEMORIJE
//ZA OVO JE ONDA POTREBNO KOD PROCESS-BR KADA SE ZOVE SOLVER DODATI I ADRESE
//PRAVIMO PREKID AKO NIJE fiksni index --- da se ne pravi kobasica bezveze
if(MemoryFlag && !VariableIndex(index))  
    _Store.ChangeValue(MEMORY, new aExp(store));
else
{
    aExp newmem = ExpArray(GetNameOfNextVariable());
    _Store.ChangeValue(MEMORY, new aExp(newmem));
    aExp addconst = aExp::Equality(newmem, store);
    _Constraints.Add(addconst);
}
}

std::string LState::GetNameOfNextVariable() const {
return GetParentBlock()->GetParentFunction()->GetNameOfNextVariable();
}

void LState::MemoryStore(LInstruction* fi, const aExp & index)
{
    
    if(!TrackPointers) return; //ovde zapravo ne bi trebalo da moze da se dodje
    llvm::Instruction* i = fi->Instruction();
    aExp value  = StoreValue(fi);
    if(GetIntType(i->getOperand(0)->getType())!=fint_type) 
        value = SetWidth(value, GetIntType(i->getOperand(0)->getType()),fint_type);
    aExp mem = *GetValue(MEMORY);
    MemoryStore(mem, index, value);
}

void LState::ProcessReturn(LInstruction* fi)
{
  llvm::Instruction* i = fi->Instruction();
//    ako je void ne radi nista
   llvm::ReturnInst *ri = llvm::dyn_cast<llvm::ReturnInst>(i);
   if(ri && (ri->getReturnValue() != NULL)&& (ri->getOperand(0)) && (ri->getOperand(0)->getType()) && (ri->getOperand(0)->getType()->getTypeID() != llvm::Type::VoidTyID))
    {
_Store.ChangeRelevant(GetOperandName(ri->getOperand(0)),true);
_Store.ChangeConnect(GetOperandName(ri->getOperand(0)),false);
}

}

void LState::ConnectFunctionArguments(llvm::Instruction* i, llvm::Function* f, unsigned numArgs, LFunction* ff)
{
  //dodaj u additional constraints prespajanje argumenata sa stvarnim argumentima
  vaExp args1;
  vaExp args2;

//FIXME sta ako je invoke
llvm::CallInst* ii = llvm::dyn_cast<llvm::CallInst>(i);

    //stvarni argumenti
  //mogu da budu promenljive a mogu da budu izrazi ili konstante
  //FIXME sta ako je pokazivac - izgleda da to radi
          for(unsigned j=0; j<numArgs; ++j)
              {
//FIXME ovo lici na getvalue!!! proveriti
//FIXME ovo je izmenjeno, ne znam zasto bas ovako treba, treba argstrat proveriti i zbaciti svuda, jer se uvek postavi na 0 pa ne sluzi nicemu, ovo je sve jako cudno a nije jasno ni sta je sa invoke
//              aExp argj = argument(i, argStart+j);
              aExp argj = argument(ii, j);
              //FIXME ovo je ako je &a za globalno proveriti ako nije globalno
              if(llvm::dyn_cast<llvm::GlobalVariable>(i->getOperand(j)))
              {
                aExp e = ExpGlobalAddress(GetOperandName(i->getOperand(j)));
                args1.push_back(e);
              }
              else
              //FIXME ako je pokazivac onda treba uzeti sadrzaj memorije sa te adrese
              //tj ako je npr izracunaj(&a) onda treba uzeti sadrzaj memorije na adresi 
              if(argj.IsVariable())
              {
                if(_Store.IsDefined(argj.GetName()))
                    { 
                    args1.push_back(*GetValue(argj.GetName())); 
                    }
                else {
                aExp e = ExpAddress(GetParentBlock()->GetFunctionName(), argj.GetName());
                args1.push_back(e);
                }
              }
              else {
                    args1.push_back(argj);  
                  }
              }
              
    //argumenti u deklaraciji
          for(llvm::Function::arg_iterator it = f->arg_begin(), ite = f->arg_end(); it!=ite; it++)
              {
              aExp e =MakeExp(VALUE, f->getName(), GetOperandName(it), 0, 0, it->getType(), ff->GetContext(), ff->GetFunctionName()); 
              args2.push_back(e);
              }

  //dodavanje u additional constraints prespajanje argumenata sa stvarnim argumentima
  for(unsigned j=0; j<numArgs; ++j)
      {
      _Constraints.Add(aExp::Equality(args1[j],args2[j]));
      }

}

void LState::ConnectFunctionMemory(llvm::Function* f, LFunction* ff)
{
  //Trenutna memorija
  aExp mem1 = *GetValue(MEMORY);
  //memorija u funkciji
  aExp mem2 = MakeExp(VALUE, f->getName(), MEMORY, 0, 0, NULL, ff->GetContext(), ff->GetFunctionName());
  //dodavanje prespajanja memorije
  _Constraints.Add(aExp::Equality(mem1,mem2));
}

aExp LState::Constraints() const 
{
    return _Constraints.Constraint();
}

//ovo je jako skupo ako ima puno uslova
//mozda bi ga trebalo spakovati nekako u jedan uslov
//ili ga nekako inkrementalno ubrzati
void LState::ConnectFunctionConditions(LInstruction* fi, LFunction* ff)
{
    GetParentBlock()->ConnectFunctionConditions(fi, ff);
}

void LState::ConnectFunctionReturnValue(llvm::Function* f, LFunction* ff, llvm::Instruction* i)
{
  //prespoj return value
  const std::vector<llvm::Instruction*> &returnInstr = ff->GetReturnInstructions();
  if(returnInstr.size()==0) 
  {
    GetParentBlock()->SetNoReturnFunctionCall();
  }


  if(returnInstr.size()>0) 
  {
    llvm::Instruction* ri = returnInstr[0];
    LBlock* fb = ff->GetLBlock(ri->getParent());
    if(fb->IsMerged()) fb = fb->MergedInto();
    assert(fb!=0);

    //izmeni memoriju
    aExp funmem = MakeExp(VALUE, f->getName(), MEMORY, fb->Id(), 1, NULL, ff->GetContext(), ff->GetFunctionName());
    _Store.ChangeValue(MEMORY, new aExp(funmem));

    llvm::ReturnInst *rri = llvm::cast<llvm::ReturnInst>(ri);
  //Ako funkcija nije void
    if(rri && (rri->getReturnValue()!=NULL) &&(rri->getOperand(0)) && (rri->getOperand(0)->getType()) && (rri->getOperand(0)->getType()->getTypeID() != llvm::Type::VoidTyID))
    {
    //Upisati povratnu vrednost
    std::string ret = GetOperandName(rri->getOperand(0));
    aExp r1 = MakeExp(VALUE, f->getName(), ret, fb->Id(), 1, rri->getType(), ff->GetContext(),ff->GetFunctionName());

    //povratna vrednost funkcije se upisuje u neki registar
    WriteIntoStore(i, r1);
//    }
//dodajemo uslov da smo morali da izadjemo iz funkcije na ovom mestu 
//tj uslov da je return blok ukljucen
//na primer ako imamo petlju mozemo da zavrsimo u mrtvom bloku i 
//onda bez ovoga se stvara greska cak i ako se doda da taj mrtvi blok ne moze 
//da bude aktivan
      aExp e = ActiveBlock(ff->GetFunctionName(), fb->Id());
      std::string s = AddContext(e.GetName(), ff->GetContext(), ff->GetFunctionName());
      _Constraints.Add(ExpFVar(s));
    }
  } 
}

void LState::ConnectGlobalVariablesBegin(llvm::Function* f, LFunction* ff)
{
std::map<std::string, LVariableInfo>::const_iterator it = _Store.GetStore().begin(), ie =
_Store.GetStore().end();
    //procitaj sve globalne vrednosti i upisi ih u store
    for( ; it!=ie; it++)
      if(isGlobalName(it->first))     
      {
        if(!ff->GetBlocks()[0]->GetState().GetStore().IsDefined(it->first)) continue;
        aExp e1 = *GetValue(it->first);
        aExp e2 = MakeExp(VALUE, f->getName(), it->first, 0, 0, it->second.Type(), ff->GetContext(), ff->GetFunctionName());        
        _Constraints.Add(aExp::Equality(e1,e2));
      }
}


void LState::ConnectGlobalVariablesEnd(llvm::Function* f, LFunction* ff)
{
  //prespoj return value
  const std::vector<llvm::Instruction*> &returnInstr = ff->GetReturnInstructions();

  if(returnInstr.size()>0) 
  {
    llvm::Instruction* ri = returnInstr[0];
    LBlock* fb = ff->GetLBlock(ri->getParent());
    if(fb->IsMerged()) fb = fb->MergedInto();
    assert(fb!=0);

std::map<std::string, LVariableInfo>::const_iterator i = fb->GetState().GetStore().GetStore().begin(), ie =
fb->GetState().GetStore().GetStore().end();
    //procitaj sve globalne vrednosti i upisi ih u store
    for( ; i!=ie; i++)
      if(isGlobalName(i->first))     
      {
        aExp e = MakeExp(VALUE, f->getName(), i->first, fb->Id(), 1,i->second.Type(), ff->GetContext(), ff->GetFunctionName());
        if(_Store.IsDefined(i->first)) _Store.ChangeValue(i->first, new aExp(e));
      }
  }
}


void LState::AddMainArgumentConditions()
{
  const llvm::Function* f = GetParentBlock()->GetParentFunction()->GetLLVMFunction();
  
  llvm::Function::const_arg_iterator argc = f->arg_begin(), end = f->arg_end(); //prvi argument
  if(argc == end) return;

  llvm::Function::const_arg_iterator argv = f->arg_begin(); ++argv; //drugi argument

  std::string name = argv->getName();
  std::string argcname = argc->getName();

  aExp argvexp = MakeExp(VALUE, GetParentBlock()->GetFunctionName(), name, 0, 0, argv->getType());
  aExp argcexp1 = MakeExp(VALUE, GetParentBlock()->GetFunctionName(), argcname, 0, 0, argc->getType());
  //ovo je jer mi je potreban broj bajtova a ne broj elemenata
  aExp expFour = ExpNum1(ItoS(fpointer_type/8), fint_type);
  aExp argcexp = aExp::mul(argcexp1, expFour);

  //Ovo je budjavstina, ali mora tako inace moze da dodje do prekoracenja ako 
  //bi na primer argc bilo oko 1 000 000 000 i zbog toga javlja uvek unsafe
  //ako se ne stavi ovakvo nekakvo ogranicenje
  aExp expBig = ExpNum1(ItoS(1000), fint_type);
  aExp e4 = aExp::slt(argcexp1, expBig);

  _Constraints.Add(e4);
  aExp e = aExp::Equality(ExpRight(argvexp), argcexp);
  _Constraints.Add(e);
  aExp e1 = aExp::Equality(ExpLeft(argvexp), ExpNumZeroInt);
  _Constraints.Add(e1);

  aExp e3 = aExp::sgt(argcexp, ExpNumZeroInt);
  _Constraints.Add(e3);
  
  //ovo je bitno za model, inace nije bitno
  aExp e5 = aExp::sgt(argvexp, ExpNumZeroInt) ; 
  _Constraints.Add(e5);

  //ovo je bitno za model, inace nije bitno
  aExp e6 = aExp::sgt(argcexp1, ExpNumZeroInt);
  _Constraints.Add(e6);
}


void LState::ProcessMemcpy(LInstruction* fi, llvm::Function* f, unsigned numArgs)
{
}

void LState::InlineFunction(LInstruction* fi, llvm::Function* f, unsigned numArgs)
{

llvm::Instruction* i = fi->Instruction();
//na ovaj nacin se za unreachable block ne racuna precizno store - ali to nije vazno jer su mu sve
//instrukcije i onako unreachable

  //pronadji LFunction koji odgovara ovoj funkciji
  LFunction* ff = GetParentBlock()->GetParentModule()->GetLFunction(f);

  //dali je funkcija sracunata
  //ako nije, izracunaj je - pretpostavka da nema rekurzije
  if(ff != NULL) 
  {
  //ovo je zastita samo od direktne rekurzije
  //FIXME treba nesto pametnije uraditi a ne samo return
    if(ff->GetFunctionName() == GetParentBlock()->GetFunctionName())  return;
//    ffTimer.startTimer();

//# ff->CalculateDescriptions();

    ff->CalculateConditions();
    ff->SetPostcondition();


//    ffTimer.stopTimer();
  } 
  else { std::cout << "funkcija nije nadjena, i sta sada???  " <<  std::endl; exit(1); }

  ff->SetNewContext();

  ConnectFunctionArguments(i, f, numArgs, ff);
  ConnectGlobalVariablesBegin(f, ff);
  ConnectFunctionMemory(f, ff);
  //ConnectCondsTimer.startTimer();
  ConnectFunctionConditions(fi, ff);
  //ConnectCondsTimer.stopTimer();
  ConnectFunctionReturnValue(f, ff, i);
  ConnectGlobalVariablesEnd(f, ff);
  ff->AddFixedAddresses(GetParentBlock()->GetParentFunction());

  //Dodaj opis funkcije u additional constraints

  //AddPostCondTimer.startTimer();
  _Constraints.Add(ff->GetPostcondition());
  //AddPostCondTimer.stopTimer();
}

  
  
void LState::ProcessFunctionCall(LInstruction* fi)
{
//fixme sta ako je invoke
  llvm::Instruction* i = fi->Instruction();

  unsigned numArgs;  
  llvm::Function *f = GetFunction(i, numArgs);
 if (!f) //FIXME ovo je vazno i treba ga srediti
  {
         std::cout << " funkcia nije deklarisana!!! " << std::endl;  
         llvm::outs() << *i << '\n';
         std::cout << " !f ????: " << std::endl;  
      return;
  } 

/*llvm::outs() << *i << '\n';
std::cout<<'\n'<< "_Store pre instrukcije poziva funkcije--------------------------" <<'\n';
_Store.Print(std::cout);
std::cout<< "PrintConstraints() pre instrukcije  poziva funkcije--------------------------" <<std::endl;
PrintConstraints();
*/

  if (f && f->isDeclaration()) 
  {

      //ako nije intrinsic onda proveravamo da li je posebna - fchange ili funroll, ako 
      //nije onda se tretira kao bibliotecni poziv
      if(f->getIntrinsicID() == llvm::Intrinsic::not_intrinsic)
        if(f->getName() == "FChange") ProcessFChange(fi, f, numArgs);
        else if(f->getName() == "FInc") ProcessFInc(fi, f, numArgs);
        else if(f->getName() == "FDec") ProcessFDec(fi, f, numArgs);
        else if(f->getName() == "FEnd") ProcessFEnd(fi, f, numArgs);
        else ProcessLibraryCall(fi, f, numArgs);
      else 
        if(f->getIntrinsicID() == llvm::Intrinsic::memcpy) ProcessMemcpy(fi, f, numArgs);
        //jeste neki drugi intrinsic ali mi to jos ne obradjujemo 
        else return; //FIXME ResetMemory();
  }
  //ovo je ako imamo negde definiciju funckije
  else 
  {
 
//InlineTimer.startTimer();
  InlineFunction(fi, f, numArgs);
//InlineTimer.stopTimer();

  //ovo je ako ne znamo sta radi funkcija zamenimo je sa novom variablom, ali treba dodati i 
  //izmenu memorije i izmenu ako su argumenti pokazivaci
  //      IntType iType = GetIntType(i->getType());
  //      aExp e = ExpVar(GetNextVariable(), iType, false);
  //      WriteIntoStore(i, e); 
  }
}

void LState::ProcessLibraryCall(LInstruction* fi, llvm::Function* f, unsigned numArgs)
{
    llvm::Instruction* i = fi->Instruction();

    if(f->getName() == "exit" )
    {
      GetParentBlock()->SetNoReturnFunctionCall();
      aExp e = GetParentBlock()->Active();
      _Constraints.Add(aExp::IMPL(e, aExp::BOT()));
    } 
    else if((f->getName() == "malloc") || (f->getName() == "realloc") 
                                       || (f->getName() == "calloc")) 
                                       //dodati i proveru tipa da je to bas the malloc a ne neki drugi
    {
//FIXME ovo moze da je greska jer se kod gepija i drugih kace expaddress a ovde ne
//      aExp e = ExpAddress(GetFunctionName(), MALLOC+GetOperandName(i));
      aExp e = ExpVar(MALLOC+GetOperandName(i), fpointer_type, false);

      WriteIntoStore(i, e); 
      aExp notzero = aExp::Disequality(e, ExpNumZeroPtr);
      aExp zero = aExp::Equality(e, ExpNumZeroPtr);
      aExp right; 
      if(f->getName() == "malloc")
          right = argument(i,0);
      else       
      if(f->getName() == "realloc")
          right = argument(i,1);
      else
      if(f->getName() == "calloc")
          right = aExp::mul(argument(i,0),argument(i,1));

      aExp eq1 = aExp::Equality(ExpLeft(e), ExpNumZeroInt);
      aExp eq2 = aExp::Equality(ExpRight(e), GetValue(right));

      aExp eqSuccess = aExp::sgt(GetValue(right), ExpNumZeroInt);    

       /* ako probamo alokaciju n*sizeof(int) onda za n=1073741825 
          i za sizeof(int) = 4, dobijamo da je alocirano 4 bajta 
          zbog prekoracenja, sto onda u petlji 
          for(i=0; i<n; i++) a[i] = 0;
          dovodi do greske. Kako iskljuciti ovo ako zelimo da 
          se ovakva prekoracenja ne prijavljuju? 
          zapravo, ovo ne treba uvek iskljuciti, vec samo nekada
          onda kada mi to zelimo da se ne prijavi
          Da bi se iskljucilo, mora da se ogranici i n i n*sizeof
          da oba budu pozitivna ali to nije dovoljno, 
          vec mora za n da se stavi neka gornja granica tako 
          da ne moze da prekoraci a isto tako i donja granica
          mozda bi ovde trebalo dodati i da n<n*sizeof() */

        if(Students)
        {
               aExp mul = GetValue(right);
              if(mul.isMul() || mul.isShiftL())
              {
                 //ovo je za varijantu malloc(k*sizeof(int)) mada je ovde 100 miliona a ne milijarda
                if(mul[0].IsVariable()) _Constraints.Add(aExp::sle(mul[0], ExpNum1(100000000, fint_type)));
                if(mul[0].IsVariable()) _Constraints.Add(aExp::sgt(mul[0], ExpNumZeroInt));
                if(mul[1].IsVariable()) _Constraints.Add(aExp::sle(mul[1], ExpNum1(100000000, fint_type)));
                if(mul[1].IsVariable()) _Constraints.Add(aExp::sgt(mul[0], ExpNumZeroInt));
              }
        }

      aExp eq3 = aExp::Equality(ExpRight(e), ExpNumZeroInt);
      _Constraints.Add(eq1);
        
aExp eq5 = aExp::AND(aExp::AND(notzero, eq2),eqSuccess) ;
      aExp Or = aExp::OR(eq5, aExp::AND(zero, eq3));
      _Constraints.Add(Or);
    }
    else if(f->getName() == "scanf" || f->getName() == "__isoc99_scanf") 
    {
      unsigned j;
      //preskacemo string koji je uvek pri argument scanf-a
      for (j=1; j<numArgs; j++)
      {
      llvm::GetElementPtrInst *gepi = llvm::dyn_cast<llvm::GetElementPtrInst>(i->getOperand(j));
      if(gepi) {
            aExp e = argument(i, j); 
            GepiArgument(e, fi);
       }
        
/*      const llvm::Type* t = i->getOperand(argStart+j)->getType();
      //ovo je zakrpa za scanf("%s", &s); jer je tu adresa od s isto sto i s, ali zbog modelovanja se napravi problem
      if(t && t->isSized() )
            continue;
            
      //proveriti da li je ovo dobro
      aExp e = ExpVar(GetNameOfNextVariable(), GetIntType(i->getOperand(argStart+j)->getType()), false);
      _Store.Write(GetOperandName(i->getOperand(argStart+j)), new aExp(e), i->getOperand(argStart+j)->getType());        */

      std::string name = GetOperandName(i->getOperand(j));
      const llvm::Type* t = _Store.GetType(name);
      if(t==NULL) {
//      std::cerr << "#### ovde je null tip, promenljiva name = "<< name <<std::endl;
//      _Store.Print(std::cerr); //fixme ima negde da se tip globalne promenljive ne postavi kako treba u storu --- trebalo bi da je int a on ga postavi na pokazivac
      std::string s = GlobalName(GetOperandName(i->getOperand(j)));
      if(_Store.IsDefined(s)) {name = s; t = _Store.GetType(name);}
      //fixme ovde moze da bude problem ako je globalna promenljiva a ne cuva se preko stora pa treba da se upise u memoriju
      }
      
      //ovo je zakrpa za scanf("%s", &s); jer je tu adresa od s isto sto i s, ali zbog modelovanja se napravi problem
        if(t && (t->getTypeID() == llvm::Type::ArrayTyID))
            {       
            continue; }
                       
      aExp e = ExpVar(GetNameOfNextVariable(), GetIntType(_Store.GetType(name)), false );
      _Store.Write(name, new aExp(e), i->getOperand(j)->getType());        

      }
    }

    else if(f->getName() == "bsearch" ) 
    {
      aExp e = ExpVar(GetNameOfNextVariable(), fpointer_type, false);
      WriteIntoStore(i, e); 

      aExp notzero = aExp::Disequality(e, ExpNumZeroPtr);
      aExp zero = aExp::Equality(e, ExpNumZeroPtr);
      aExp right = argument(i,3);
      aExp eq1 = aExp::Equality(ExpLeft(e), ExpNumZeroInt);
      aExp eq2 = aExp::Equality(ExpRight(e), GetValue(right));
      aExp eq3 = aExp::Equality(ExpRight(e), ExpNumZeroInt);
      aExp arr = argument(i, 1);
      aExp size = argument(i, 2);
      aExp eqValueNotNull1 = SimplifyExpression(aExp::sge(e, arr)); 
      aExp eqValueNotNull2 = SimplifyExpression(aExp::slt(e, SimplifyExpression(aExp::add(arr, size)))); 
      _Constraints.Add(eq1);
      aExp notNull = aExp::AND(notzero, eq2);
      notNull = aExp::AND(notNull, eqValueNotNull1);
      notNull = aExp::AND(notNull, eqValueNotNull2);
      aExp Or = aExp::OR(notNull, aExp::AND(zero, eq3));
      _Constraints.Add(Or);
    }
    else if(f->getName() == "free" ) 
    {
      aExp ee = *(GetValue(GetOperandName(i->getOperand(0))));
      aExp zero = aExp::Equality(ee, ExpNumZeroPtr);
      _Constraints.Add(zero);
      aExp eq1 = aExp::Equality(ExpLeft(ee), ExpNumZeroInt);
      aExp eq3 = aExp::Equality(ExpRight(ee), ExpNumZeroInt);
      _Constraints.Add(eq1);
      _Constraints.Add(eq3);
    }
    else if(f->getName() == "strcpy" ) //ovo je sumnjivo, poredjenje je samo na osnovu alocirane memorije a ne na osnovu stvarnog stanja u memoriji
    {
          aExp dst = GetValue(argument(i,0));
          aExp src = GetValue(argument(i,1));
          aExp e = aExp::uge(ExpRight(dst), ExpRight(src));
          GetParentBlock()->AddLocalCondition(e, fi, BUFFEROVERFLOW);    
    }
//FIXME ovde bi trebalo modelirati da je argument razlicit od null
    else if(f->getName() == "_IO_getc" || f->getName() == "_IO_fgetc") 
    {
      aExp e = ExpVar(GetNameOfNextVariable(), fchar_type, false);
      WriteIntoStore(i, e); 
    }
else if(f->getName() == "getchar")
{
      aExp e = ExpVar(GetNextVariable(), fint_type, false);
      WriteIntoStore(i, e); 
}
    else if((f->getName() == "ASSERT") || (f->getName() == "assert") || (f->getName() == "ASSERT_") || (f->getName() == "assert_")) 
    {

      if(CheckAssert)
      {
      aExp r = GetValue(argument(i,0));
      if(r.isZext()){
        GetParentBlock()->AddLocalCondition(r[0],fi, ASSERT);
    }
      else
        {
          const llvm::Type* t = (fi->Instruction())->getType();
          aExp e = aExp::Disequality(r, ExpNumZero(GetIntType(t)));
          GetParentBlock()->AddLocalCondition(e, fi, ASSERT);
        }
      }
    }
    else if((f->getName() == "ASSUME") || (f->getName() == "assume") || (f->getName() == "ASSUME_") || (f->getName() == "assume_")) 
    {
      aExp cond = GetValue(argument(i,0));
      if(cond.isZext())
        _Constraints.Add(cond[0]);
      else
      {
          const llvm::Type* t = i->getOperand(0)->getType();
          aExp e = aExp::Disequality(cond, ExpNumZero(GetIntType(t)));
        _Constraints.Add(e);
      }
    }
//#ifdef Z3 --- u common prebaciti izbor solvera i ovde promeniti solver po potrebi
//ovo moze ako se ne radi akermanizacija jer je za ovo suludo raditi akermanizaciju
else if(f->getName() == "cos")
{
      aExp e = aExp::Function("cos", GetValue(argument(i,0)), GetIntType(i->getType()));
      WriteIntoStore(i, e); 
}
else if(f->getName() == "toupper")
{
      aExp e = aExp::Function("toupper", GetValue(argument(i,0)), GetIntType(i->getType()));
      WriteIntoStore(i, e); 
}
else if(f->getName() == "tolower")
{
      aExp e = aExp::Function("tolower", GetValue(argument(i,0)), GetIntType(i->getType()));
      WriteIntoStore(i, e); 
}
else if(f->getName() == "atoi")
{
      aExp e = aExp::Function("atoi", GetValue(argument(i,0)), GetIntType(i->getType()));
      WriteIntoStore(i, e); 
}

else 
{
      aExp e = ExpVar(GetNameOfNextVariable(), GetIntType(i->getType()), false);
      WriteIntoStore(i, e); 
//  ResetMemory();
}

//#endif
  //ovo je kada ne znamo nista o funkciji
/*  else 
  {
  ResetMemory();
  }
*/
}

void LState::ProcessGEPI(LInstruction* fi)
{
  llvm::Instruction* i = fi->Instruction();
  llvm::GetElementPtrInst *gepi = llvm::cast<llvm::GetElementPtrInst>(i);

  aExp base = *GetValue(GetOperandName(gepi->getOperand(0)));
    if(gepi->getOperand(0)->hasName() )
    {
//fixmeT izmenjeno allocationinst
        llvm::AllocaInst* ai = llvm::dyn_cast<llvm::AllocaInst>(gepi->getOperand(0));
        if(ai)
        {
          if(gepi->getOperand(0)->getType() != ai->getAllocatedType())
              {
              base = ExpAddress(GetParentBlock()->GetFunctionName(), GetOperandName(gepi->getOperand(0)));
              }
        }
    }
 
  aExp addend = ExpNumZeroInt;
  aExp constantOffset = ExpNumZeroInt;

  if(base.isAdd()) 
  {
    aExp gepi = base; base = gepi[0];
    if(GetBitWidth(fint_type) != GetBitWidth(fpointer_type))
      constantOffset = gepi[1][0];
    else 
      constantOffset = gepi[1];
  }

  for (llvm::gep_type_iterator ii = llvm::gep_type_begin(gepi), ie = llvm::gep_type_end(gepi); ii != ie; ++ii) 
  {
    if (llvm::StructType *st = llvm::dyn_cast<llvm::StructType>(*ii)) {
      const llvm::StructLayout *sl = GetParentBlock()->GetParentModule()->GetTargetData()->getStructLayout(st);
      const llvm::ConstantInt *ci = llvm::cast<llvm::ConstantInt>(ii.getOperand());
      uint64_t iaddend = sl->getElementOffset((unsigned) ci->getZExtValue());
      addend = ExpNum1(int(iaddend), fint_type);
    } else {
      const llvm::SequentialType *sty = llvm::cast<llvm::SequentialType>(*ii);
      unsigned elementSize = 
        GetParentBlock()->GetParentModule()->GetTargetData()->getTypeStoreSize(sty->getElementType());
      llvm::Value *operand = ii.getOperand();
      if (llvm::Constant *c = llvm::dyn_cast<llvm::Constant>(operand)) {
        aExp e = evalConstant(c);
        aExp index = ToInt(e, e.getIntWidth());
        addend = SimplifyExpression(aExp::mul(index, ExpNum1(elementSize, fint_type)));
      } else {
        const aExp* index = GetValue(GetOperandName(operand)); 
        const llvm::Type* t = _Store.GetType(GetOperandName(operand));
        unsigned int width = GetBitWidth(GetIntType(t));
        aExp e = ToInt(*index, width);
        addend = SimplifyExpression(aExp::mul(e, ExpNum1(elementSize, fint_type)));
      }
    }

    constantOffset = SimplifyExpression(aExp::add(constantOffset, addend));
  }//kraj fora

  aExp c = IntToPtr(constantOffset);
  aExp ee = ExpGepi(base, c);  
  WriteIntoStore(i, ee);

  //FIXME mozda je logicnije da se ova ogranicenja dodaju tek kada se dereferencira ova adresa
  //jer su ona potpuno nebitna ukoliko dereferenciranja nema --- takodje, ako ovo dovodi do greske
  //onda ga mozda ne treba dodati da bi sistem mogao da se od toga oporavi i nastavi dalje da ne bi 
  //bilo posle sve unreachable
  aExp sright = aExp::sub(ExpRight(base), constantOffset);
  aExp e1 = aExp::Equality(ExpRight(ee), sright);

  aExp sleft = aExp::sub(ExpLeft(base), constantOffset);
  aExp e2 = aExp::Equality(ExpLeft(ee), sleft);

  _Constraints.Add(e1);
  _Constraints.Add(e2);

/*std::cout<<'\n'<< "posle instrukcije gepi--------------------------" <<'\n';
_Store.Print(std::cout);
std::cout<< "posle instrukcije gepi--------------------------" <<std::endl;
PrintConstraints();
*/
}

//TODO
void LState::ProcessPHI(LInstruction* fi)
{
// fi ce da ima suvisne informacije --- duplo ce se prespajati neke vrednosti 
//to bi trebalo izbaciti da se ne prespaja dva puta --- nema smisla
//nece da radi getelementptr ako mu je prethodnik phi on ne ume da odredi 
//koliki su mu left i right

    llvm::Instruction* i = fi->Instruction();

    std::string value  = GetNextVariable();
    aExp e = ExpVar(value, GetIntType(i->getType()), false);
    WriteIntoStore(i, e);

	//ako ima samo jednog predhodnika
      if(_ParentBlock->PredsNumber() == 1)
	    {
	    
	    LBlock* pred = _ParentBlock->GetPred();

        for(unsigned j=0; j+1<i->getNumOperands(); j+=2)
        {
          llvm::BasicBlock* bb1 = llvm::dyn_cast<llvm::BasicBlock>(i->getOperand(j+1));
          assert(bb1!=NULL);
          LBlock* fbb1 = _ParentBlock->GetParentFunction()->GetLBlock(bb1);
          if(fbb1 != pred) {continue;}	  
          //dodati u store ovu vrednost 

          //FIXME ovo je sumnjivo, mora da postoji neki metod za citanje 
          //ove vrednosti i treba ga naci gde je, ovo je copy-paste od onoga dole
          aExp e3;
          llvm::Constant* c1 = llvm::dyn_cast<llvm::Constant>(i->getOperand(j));
          if(c1!=NULL) e3 = evalConstant(c1);
          else {
            std::string s = GetOperandName(i->getOperand(j));
            if(GetValue(s) != NULL) e3 = *GetValue(s);
            else {
              //FIXME tip
              e3 = ExpVar(s, argo::sInt32Type, false); 
            }
          } 
         WriteIntoStore(i, e3);
    	   break;
	      }
	    }
	    else{
//ako imamo vise od jednog prethodnika
llvm::PHINode *phiin = static_cast<llvm::PHINode*>(i);

        vaExp options;
          for(unsigned j=0; j<phiin->getNumIncomingValues(); j++)
          {
            llvm::BasicBlock* bb1 = llvm::dyn_cast<llvm::BasicBlock>(phiin->getIncomingBlock(j));
            assert(bb1!=NULL && "bb1==NULL");
            LBlock* pred = _ParentBlock->GetParentFunction()->GetLBlock(bb1);


            //ako su neki blokovi sracunati da ne mogu da budu predhodnici onda ih preskociti
            //ne treba preskociti uvek, ako su spojeni uzeti ono u sta su spojeni
            if(_ParentBlock->GetPredWithId(pred->Id()) == NULL) { 
                if(pred->IsMerged()) pred = pred->MergedInto();
                    else continue; }

            aExp e1 = Transformation( _ParentBlock->GetFunctionName(), pred->Id(), _ParentBlock->Id());

            aExp e3;

            llvm::Constant* c1 = llvm::dyn_cast<llvm::Constant>(phiin->getIncomingValue(j));
            if(c1!=NULL) {e3 = evalConstant(c1);}
            else {
                std::string s = GetOperandName(phiin->getIncomingValue(j));
                if(GetValue(s) != NULL) {//std::cout << "S = " << s << std::endl; _Store.Print(std::cout); 
                                         e3 = *GetValue(s); }
                else e3 = ExpVar(s, GetIntType(phiin->getIncomingValue(j)->getType()), false); 
            }

            aExp e2 = aExp::Equality(ExpVar(value, GetIntType(i->getType()), false), e3);
            aExp impl = aExp::IMPL(e1, e2);
            AddConstraint(impl);
          }
}//od else
}


//FIXME ako ima noreturn function call
void LState::ProcessSwitch(LInstruction* fi)
{
  llvm::Instruction* i = fi->Instruction();
  llvm::SwitchInst *si = llvm::cast<llvm::SwitchInst>(i);
  llvm::Value* cond = si->getCondition();

  aExp arg1;

  //treablo bi da uvek ode na else jer ako je konstanta to se automatski pojednostavi
  if(llvm::Constant *c = llvm::dyn_cast<llvm::Constant>(cond)) 
      arg1 = evalConstant(c);
  //GetValue gleda samo ime promenljive tako da tip i relevant nisu uopste vazni
  else arg1 = GetValue(ExpVar(GetOperandName(cond), argo::sInt32Type, false));

  vaExp defExp;
  std::vector<bool> added;
  llvm::SwitchInst::CaseIt cit = si->case_begin(), cite = si->case_end();

  for(; cit!=cite; ++cit)
        added.push_back(false);

//  for(unsigned j=0; j<si->getNumSuccessors(); j++)
//      added.push_back(false);


  cit = si->case_begin();
  ++cit;
  unsigned j = 1;
  for(; cit!=cite; ++cit, j++)
  {
    if(added[j]) continue;
    //ako je jedinstvani case-dest
    if(si->findCaseDest(si->getSuccessor(j)))
    {
      added[j] = true;
//      llvm::ConstantInt* ci = si->getSuccessorValue(j);
      llvm::ConstantInt* ci = cit.getCaseValue();

      aExp arg2 = ExpNum1(ci->getValue().toString(10,true), fint_type);
      aExp e = aExp::Equality(arg1, arg2);

      GetParentBlock()->AddJump(LJump(e, si->getSuccessor(j)));
      //pamti se uslov za default opciju
      defExp.push_back(aExp::NOT(e));
    }
    //ako nije jedinstven case-dest
    else
    {
      vaExp args;
      args.push_back(aExp::Equality(arg1, ExpNum1(cit.getCaseValue()->getValue().toString(10,true), arg1.getIntType())));
      added[j] = true;
      llvm::SwitchInst::CaseIt cit1 = si->case_begin();//, cite1 = si->case_end();
      for(unsigned k=1; k<si->getNumSuccessors(); k++,++cit1)
      {
        if(added[k]) continue;
        if(si->getSuccessor(j) == si->getSuccessor(k))
        {
          args.push_back(aExp::Equality(arg1, ExpNum1(cit1.getCaseValue()->getValue().toString(10,true), arg1.getIntType())));
          added[k] = true;
        }
      }

      aExp e = aExp::OR(args);
      GetParentBlock()->AddJump(LJump(e, si->getSuccessor(j)));
      //pamti se uslov za default opciju
      defExp.push_back(aExp::NOT(e));
    }//end of else
  }//end of for(; cit!=cite; ++cit, j++)

   //dodaje se jump za default opciju - tu je uslov da nije ispunjen ni jedan 
   //od prethodnih uslova
  GetParentBlock()->AddJump(LJump(aExp::AND(defExp), si->getDefaultDest()));
}

void LState::ProcessBr(LInstruction* fi)
{
  if(_ParentBlock->HasNoReturnFunctionCall()) return;

  const llvm::Instruction* i = fi->Instruction();
  const llvm::BranchInst *bi = llvm::cast<llvm::BranchInst>(i);
  if(bi->isUnconditional())
    _ParentBlock->AddJump(LJump(bi->getSuccessor(0)));
  else
  {
  aExp arg0 = argument(i,0);
 
//ovo je mnogo efikasnije ali moze da napravi gresku, tj da se lokalno nesto dokaze a da globalno ne bi moglo jer se ovde previdi da npr nije dopusten ulaz u petlju ili nije dopusten ulaz u neko if
  aExp t = aExp::TOP();
  aExp check = GetValue(arg0);
  STATUS s = UNSAFE;
  if(check.IsTOP())  { s = SAFE;}
  else if(check.IsBOT()) { s = FLAWED;}
  else {
          if(MemoryFlag)
              _ParentBlock->UpdateAndSetAddresses();
          s = LSolver::callSolver(t, check);
  }

  LFunction* f=_ParentBlock->GetParentFunction();
  if((s == SAFE) || (s == FLAWED))
  {
      if(s==SAFE)
      {  
        _ParentBlock->AddJump(LJump(GetValue(arg0), bi->getSuccessor(0)));
        LBlock* fb = f->GetLBlock(bi->getSuccessor(1));
        fb->AddNoPred(_ParentBlock);
      }
      else
      {
        _ParentBlock->AddJump(LJump(aExp::NOT(GetValue(arg0)), bi->getSuccessor(1)));
        LBlock* fb = f->GetLBlock(bi->getSuccessor(0));
        fb->AddNoPred(_ParentBlock);
      }
  }   
  //ovo je ako ne moze ni jedna grana da se otkine
  else
  {
      _ParentBlock->AddJump(LJump(GetValue(arg0), bi->getSuccessor(0)));
      _ParentBlock->AddJump(LJump(aExp::NOT(GetValue(arg0)), bi->getSuccessor(1)));
  }
  }

}

static std::string cut_addr(const std::string& s)
{
std::string result = s;
result.erase(result.size()-5); // jer _addr ima 5 karaktera
return result;
}





//ovde bi bilo bolje da se jednom sracunaju ove vrednosti a ne svaki put iz pocetka
//problem je sto u zavisnosti od pozicije u bloku mozemo imati razlicite vrednosti
//koje nam za model trebaju i to zavisi od trenutnog stanja izvrsavanja instrukcija
//to komplikuje stvari i ne znam kako bi to razresila drugacije nego da se svaki
//put iznova izracunava
mspaExp LState::GetModelValues() const
{
const msVarInfo& st = _Store.GetStore();
mspaExp ModelValues;

msVarInfo::const_iterator it = st.begin(), ite=st.end();
std::set<std::string> skip; 
for( ;it!=ite; it++ )
    if(EndsWith(it->first, "_addr")) skip.insert(cut_addr(it->first)); 

it = st.begin();
for( ;it!=ite; it++ )
{
  if(StartsWith(it->first, MEMORY)) continue;
  if(StartsWith(it->first, VARNAME)) continue;
  if(isArray(it->first)) continue;
  //ovo je adresa od argumenata funkcije, skroz je budjavo
  //sugavo je sto je ovde pretpostavka da ce llvm tako da ih nazove
  if(EndsWith(it->first, "_addr")) continue;
  if(skip.find(it->first)!=skip.end()) {continue;}
  //ovo je retvalue koje pravi llvm, i ovo je budjavo ali ne treba u modelu
  if(StartsWith(it->first, "retval")) continue;
  ModelValues.insert(std::pair<std::string, const aExp*>(it->first, it->second.Value()));
}  
return ModelValues;
}

//ovo bi bilo bolje da se radi prilikom exporta, da bi formula uvek bila ista i 
//da bi se razlikovala tek na kraju
aExp LState::GetDivValue(llvm::Instruction* i, bool b)
{
    aExp e;
    aExp e0 = GetValue(argument(i,0));
    aExp e1 = GetValue(argument(i,1));
    if(LSolver::isLATheory() && e1.IsNumeral() && !e0.IsNumeral()) 
    {
      //ako su oba numerali to ce se izracunati prilikom upisivanja u store
      // x/3 ===> y /\ 3y=x 
      //ovo ima smisla samo za realne brojeve
      //fixme tip
      //ovo je y
      e = ExpVar(GetNameOfNextVariable(), GetIntType(i->getOperand(1)->getType()), false);
      aExp ee = aExp::Equality(aExp::mul(e1, e), e0);
      _Constraints.Add(ee);
    } 
    else if(b) e = aExp::udiv(GetValue(argument(i,0)), GetValue(argument(i,1)));
          else e = aExp::sdiv(GetValue(argument(i,0)), GetValue(argument(i,1)));
return e;
}
/*ovo ne pomaze jer se mnozenje i sabiranje tretiraju realno pa to onda nije dovoljno
jednoznacno
aExp LState::GetRemValue(llvm::Instruction* i, bool b)
{
    aExp e;
    aExp e0 = GetValue(argument(i,0));
    aExp e1 = GetValue(argument(i,1));
    if(LSolver::isLATheory() && e1.IsNumeral() && !e0.IsNumeral()) 
    {
      //ako su oba numerali to ce se izracunati prilikom upisivanja u store
      //a%2 ===> b /\ a=2x+b /\ 0<=b<2
      //fixme tip
      //ovo je b
      e = ExpVar(GetNameOfNextVariable(), GetIntType(i->getOperand(1)->getType()), false);
      aExp x = ExpVar(GetNameOfNextVariable(), GetIntType(i->getOperand(1)->getType()), false);
      aExp eq1 = aExp::Equality(aExp::mul(aExp::mul(e1, x), e), e0);
      //fixme tip, fixme sle ili ule
      aExp eq2 = aExp::sle(ExpNumZeroInt, e);
      aExp eq3 = aExp::slt(e, e1);
      _Constraints.Add(eq1);
      _Constraints.Add(eq2);
      _Constraints.Add(eq3);
    } 
    else if(b) e = aExp::urem(GetValue(argument(i,0)), GetValue(argument(i,1)));
          else e = aExp::srem(GetValue(argument(i,0)), GetValue(argument(i,1)));
return e;
}*/


//FIXME - nepodrzane su free, vaarg, unwind, unreachable i neke float i poslednje tri 
void LState::Update(LInstruction* fi)
{
  if(_ParentBlock->IsUnreachableBlock()) {std::cerr << "unreachable blok!!!" << std::endl; return;}

  llvm::Instruction* i = fi->Instruction();

  switch (i->getOpcode()) {
  // Control flow
  case llvm::Instruction::Ret: {
    ProcessReturn(fi);
    break;
  }
//  case llvm::Instruction::Unwind: {
//      exit_error("Unwind instruction not supported!");
//	  std::cout << *i <<  std::endl;
//    break;
//  }
  case llvm::Instruction::Br: {
    Branching.startTimer();
    ProcessBr(fi);
    Branching.stopTimer();

    break;
  }
  case llvm::Instruction::Switch: {
   ProcessSwitch(fi);
   break;
  }

  case llvm::Instruction::Unreachable: {
//	  std::cout << *i <<  std::endl;
    break;
  }

  case llvm::Instruction::Invoke:
  case llvm::Instruction::Call: {
    ProcessFunctionCall(fi);
    break;
  }

  case llvm::Instruction::PHI: {
    ProcessPHI(fi);
    break;
  }

  // Special instructions
  case llvm::Instruction::Select: {
    ProcessSelect(fi);
    break;
  }

  case llvm::Instruction::VAArg:{
    exit_error("VAArg not supported!");
    break;
  }



  ///////////////////////
  // Arithmetic / logical
  case llvm::Instruction::Add: {
    aExp e = aExp::add(GetValue(argument(i,0)), GetValue(argument(i,1)));
    WriteIntoStore(i,e);
    break;
  }

  case llvm::Instruction::Sub: {
    aExp e = aExp::sub(GetValue(argument(i,0)), GetValue(argument(i,1)));
    WriteIntoStore(i,e);
    break;
  }
 
  case llvm::Instruction::Mul: {
    aExp e = aExp::mul(GetValue(argument(i,0)), GetValue(argument(i,1)));
    WriteIntoStore(i,e);
    break;
  }


  case llvm::Instruction::UDiv: {
    GetParentBlock()->AddLocalConditionZeroDisequality(GetValue(argument(i,1)), fi);
    aExp e = GetDivValue(i, true);
    WriteIntoStore(i,e);
    break;
  }

  case llvm::Instruction::SDiv: {
    GetParentBlock()->AddLocalConditionZeroDisequality(GetValue(argument(i,1)), fi);
    aExp e = GetDivValue(i, false);
    WriteIntoStore(i,e);
    break;
  }

  case llvm::Instruction::URem: {
    GetParentBlock()->AddLocalConditionZeroDisequality(GetValue(argument(i,1)), fi);
    aExp e = aExp::urem(GetValue(argument(i,0)), GetValue(argument(i,1)));
//    aExp e = GetRemValue(i, true);
    WriteIntoStore(i,e);
    break; 
  }
 
  case llvm::Instruction::SRem: {
    GetParentBlock()->AddLocalConditionZeroDisequality(GetValue(argument(i,1)), fi);
    aExp e = aExp::srem(GetValue(argument(i,0)), GetValue(argument(i,1)));
//    aExp e = GetRemValue(i, false);
    WriteIntoStore(i,e);
    break;
  }

  case llvm::Instruction::And: {
    aExp e = aExp::bitAnd(GetValue(argument(i,0)), GetValue(argument(i,1)));
    WriteIntoStore(i,e);
    break;
  }

  case llvm::Instruction::Or: {
    aExp e = aExp::bitOr(GetValue(argument(i,0)), GetValue(argument(i,1)));
    WriteIntoStore(i,e);
    break;
  }

  case llvm::Instruction::Xor: {
    ProcessXor(fi);
    break;
  }

  case llvm::Instruction::Shl: {
    aExp e = aExp::ShiftL(GetValue(argument(i,0)), GetValue(argument(i,1)));
    WriteIntoStore(i,e);
    break;
  }

  //logical shift - za unsigned
  case llvm::Instruction::LShr: {
    aExp e = aExp::lShiftR(GetValue(argument(i,0)), GetValue(argument(i,1)));
    WriteIntoStore(i,e);
    break;
  }

  //arithmetic shift - za signed
  case llvm::Instruction::AShr: {
    aExp e = aExp::aShiftR(GetValue(argument(i,0)), GetValue(argument(i,1)));
    WriteIntoStore(i,e);
    break;
  }

  // Compare
  case llvm::Instruction::ICmp: {
    ProcessICmp(fi);
    break;
  }
 
  // Memory instructions...
  case llvm::Instruction::Alloca:
//  case llvm::Instruction::Malloc: 
  {
    ProcessAllocation(fi);
   break;
  }
  
//fixmeT videti gde je ovo nestalo
  //fixme ovo je skroz neprovereno
/*  case llvm::Instruction::Free: {
//  std::cout << *i <<  std::endl;
      aExp ee = GetValue(argument(i,0));
      aExp zero = aExp::Equality(ee, ExpNumZeroPtr);
      _Constraints.Add(zero);
      aExp eq1 = aExp::Equality(ExpLeft(ee), ExpNumZeroInt);
      aExp eq3 = aExp::Equality(ExpRight(ee), ExpNumZeroInt);
      _Constraints.Add(eq1);
      _Constraints.Add(eq3);
    break;
  }
  
*/
  case llvm::Instruction::Load: {
    ProcessLoad(fi);
    break;
  }

  case llvm::Instruction::Store: {
    ProcessStore(fi);
    break;
  }

  case llvm::Instruction::GetElementPtr: {
    ProcessGEPI(fi);
    break;
  }

  // Conversion
  case llvm::Instruction::Trunc: {
    aExp e0 = GetValue(argument(i,0));
    unsigned width = GetBitWidth(GetIntType(i->getType()));
    aExp e;// = aExp::extract(e0, 0, width-1); 
    if(e0.IsNumeral()) e = ExpNum1(e0.GetValue(), GetIntType(i->getType()));
    else e = aExp::extract(e0, 0, width-1);
    WriteIntoStore(i, e);
    break;
  }

  case llvm::Instruction::ZExt: {
    aExp e0 = GetValue(argument(i,0));
    unsigned width = GetBitWidth(GetIntType(i->getType()));
    aExp e;// = aExp::zext(e0, width);
    if(e0.IsNumeral()) e = ExpNum1(e0.GetValue(), GetIntType(i->getType()));
    //ovo je vazno za z3, za boolector nije potrebno, pitanje je jedino da li
    //su potrebni ovi drugi ili je equality dovoljno
    else if(e0.IsEquality()||e0.IsDisequality()
    ||e0.isUle()||e0.isUlt()||e0.isUge()||e0.isUgt()
    ||e0.isSle()||e0.isSlt()||e0.isSge()||e0.isSgt())
         e = aExp::IfThenElse(e0, ExpNum1("1", GetIntType(width)), ExpNumZero(GetIntType(width)));
    else if(e0.IsTOP()) e = ExpNum1("1", GetIntType(width));
    else if(e0.IsBOT()) e = ExpNumZero(GetIntType(width));
    else e = aExp::zext(e0, width);
    WriteIntoStore(i, e);
    break;
  }

  case llvm::Instruction::SExt: {
    aExp e0 = GetValue(argument(i,0));
    unsigned width = GetBitWidth(GetIntType(i->getType()));
    aExp e;// = aExp::sext(e0, width);
    if(e0.IsNumeral()) e = ExpNum1(e0.GetValue(), GetIntType(i->getType()));
    else e = aExp::sext(e0, width);
    WriteIntoStore(i, e);
    break;
  }

  case llvm::Instruction::IntToPtr: {
    ProcessIntToPtr(fi);
    break;
  } 
  case llvm::Instruction::PtrToInt: {
    ProcessPtrToInt(fi);
    break;
  }

  case llvm::Instruction::BitCast: {
    ProcessBitCast(fi);
    break;          

  }
    // Floating point specific instructions
  case llvm::Instruction::FAdd: {
//    BinaryOperator *bi = cast<BinaryOperator>(i);
//	  std::cout << *bi <<  std::endl;
    aExp e = aExp::add(GetValue(argument(i,0)), GetValue(argument(i,1)));
    WriteIntoStore(i,e);
    break;
  }

  case llvm::Instruction::FSub: {
//    BinaryOperator *bi = cast<BinaryOperator>(i);
    aExp e = aExp::sub(GetValue(argument(i,0)), GetValue(argument(i,1)));
    WriteIntoStore(i,e);
    break;
  }
 
  case llvm::Instruction::FMul: {
//    BinaryOperator *bi = cast<BinaryOperator>(i);
    aExp e = aExp::mul(GetValue(argument(i,0)), GetValue(argument(i,1)));
    WriteIntoStore(i,e);

    break;
  }

  case llvm::Instruction::FDiv: {
//  BinaryOperator *bi = cast<BinaryOperator>(i);
    GetParentBlock()->AddLocalConditionZeroDisequality(GetValue(argument(i,1)), fi);
//    aExp e = aExp::sdiv(GetValue(argument(i,0)), GetValue(argument(i,1)));
      aExp e = GetDivValue(i, false);
    WriteIntoStore(i,e);
    break;
  }

  case llvm::Instruction::FRem: {
//  BinaryOperator *bi = cast<BinaryOperator>(i);
    GetParentBlock()->AddLocalConditionZeroDisequality(GetValue(argument(i,1)), fi);
    aExp e = aExp::srem(GetValue(argument(i,0)), GetValue(argument(i,1)));
    WriteIntoStore(i,e);
    break;
  }

  //fixme
  case llvm::Instruction::FPTrunc: {
//  FPTruncInst *fi = cast<FPTruncInst>(i);
    aExp e0 = GetValue(argument(i,0));
    WriteIntoStore(i, e0);
    break;
  }

  //fixme
  case llvm::Instruction::FPExt: {
//    FPExtInst *fi = cast<FPExtInst>(i);
    aExp e0 = GetValue(argument(i,0));
    WriteIntoStore(i, e0);
    break;
  }

  case llvm::Instruction::FPToUI: {
//  FPToUIInst *fi = cast<FPToUIInst>(i);
//    aExp e0 = GetValue(argument(i,0));
//    WriteIntoStore(i, e0);
    aExp e0 = GetValue(argument(i,0));
    unsigned width = GetBitWidth(GetIntType(i->getType()));
    aExp e;// = aExp::zext(e0, width);
    if(e0.IsNumeral()) e = ExpNum1(e0.GetValue(), GetIntType(i->getType()));
    else e = aExp::zext(e0, width);
    WriteIntoStore(i, e);
    break;
  }

  case llvm::Instruction::FPToSI: {
//  FPToSIInst *fi = cast<FPToSIInst>(i);
//    aExp e0 = GetValue(argument(i,0));
//    WriteIntoStore(i, e0);
    aExp e0 = GetValue(argument(i,0));
    unsigned width = GetBitWidth(GetIntType(i->getType()));
    aExp e;// = aExp::zext(e0, width);
    if(e0.IsNumeral()) e = ExpNum1(e0.GetValue(), GetIntType(i->getType()));
    else e = aExp::zext(e0, width);
    WriteIntoStore(i, e);

    break;
  }

  case llvm::Instruction::UIToFP: {
//  UIToFPInst *fi = cast<UIToFPInst>(i);
//    aExp e0 = GetValue(argument(i,0));
//    WriteIntoStore(i, e0);

    aExp e0 = GetValue(argument(i,0));
    unsigned width = GetBitWidth(GetIntType(i->getType()));
    aExp e;// = aExp::zext(e0, width);
    if(e0.IsNumeral()) e = ExpNum1(e0.GetValue(), GetIntType(i->getType()));
    else e = aExp::zext(e0, width);
    WriteIntoStore(i, e);

    break;
  }

  case llvm::Instruction::SIToFP: {
//  SIToFPInst *fi = cast<SIToFPInst>(i);
//   aExp e0 = GetValue(argument(i,0));
//   WriteIntoStore(i, e0);
 
    aExp e0 = GetValue(argument(i,0));
    unsigned width = GetBitWidth(GetIntType(i->getType()));
    aExp e;// = aExp::zext(e0, width);
    if(e0.IsNumeral()) e = ExpNum1(e0.GetValue(), GetIntType(i->getType()));
    else e = aExp::zext(e0, width);
    WriteIntoStore(i, e);

    break;
  }

  case llvm::Instruction::FCmp: {
//  FCmpInst *fi = cast<FCmpInst>(i);
    ProcessFCmp(fi);
    break;
  }
 
  // Other instructions...
  // Unhandled
  case llvm::Instruction::ExtractElement:
  case llvm::Instruction::InsertElement:
  case llvm::Instruction::ShuffleVector:
    exit_error("Unsuported instructions: ExtractElement, InsertElement, ShuffleVector!");
//    std::cout << *i <<  std::endl;
    break;

  default:
        std::cout << "nepodrzano" << std::endl;
//    exit_error("unknown instruction");
    break;
  }//end of switch
}//end of function


//ne moze zbog gettargetdata da se jednostavno izbaci iz bloka u common
//FIXME pola toga je zbrljano ili preskoceno!!!
aExp LState::evalConstantExpr(llvm::ConstantExpr *ce)
{
std::cout << std::endl<< std::endl<< std::endl<< std::endl<< std::endl<< std::endl<< std::endl<< std::endl<< std::endl<< std::endl<< "evalConstantExpr ---------------------------------------" << std::endl;
//std::cout << std::endl << "!!!!!!!!!!!!!!!ovde moze da je greska123: evalConstantExpr \n" << *ce << std::endl ;

//aExp op1=ExpZero, op2 = ExpZero, op3 = ExpZero;
//FIXME!!!
aExp op1=ExpNumZeroInt, op2 = ExpNumZeroInt, op3 = ExpNumZeroInt;

int numOperands = ce->getNumOperands();

//std::cout << "*ce->getOperand(0) " << *ce->getOperand(0) << std::endl;
if (numOperands > 0) op1 = evalConstant(ce->getOperand(0));
if (numOperands > 1) op2 = evalConstant(ce->getOperand(1));
if (numOperands > 2) op3 = evalConstant(ce->getOperand(2));
std::cout << "evalConstantExpr: op1 ";  op1.Print(&SMTF, std::cout); std::cout << std::endl;
std::cout << "evalConstantExpr: op2 ";  op2.Print(&SMTF, std::cout); std::cout << std::endl;
std::cout << "evalConstantExpr: op3 ";  op3.Print(&SMTF, std::cout); std::cout << std::endl;

switch (ce->getOpcode()) {
default : 
        {
        ce->dump();
        std::cerr << "error: unknown ConstantExpr type\n"
        << "opcode: " << ce->getOpcode() << "\n";
        abort();
        break;
        }
//FIXME
case llvm::Instruction::Trunc: 
case llvm::Instruction::ZExt:  
case llvm::Instruction::SExt:  
{
//  std::cout << std::endl << "Trunc ZExt i SExt --- ovde moze da je puklo" << std::endl;
  return op1;
}
case llvm::Instruction::Add:   return (SimplifyExpression(aExp::add(op1, op2)));
case llvm::Instruction::Sub:   return (SimplifyExpression(aExp::sub(op1, op2)));
case llvm::Instruction::Mul:   return (SimplifyExpression(aExp::mul(op1, op2)));
case llvm::Instruction::SDiv:  return (SimplifyExpression(aExp::sdiv(op1, op2)));
case llvm::Instruction::UDiv:  return (SimplifyExpression(aExp::udiv(op1, op2)));
case llvm::Instruction::SRem:  return (SimplifyExpression(aExp::srem(op1, op2)));
case llvm::Instruction::URem:  return (SimplifyExpression(aExp::urem(op1, op2)));
case llvm::Instruction::And:   return (SimplifyExpression(aExp::bitAnd(op1, op2)));
case llvm::Instruction::Or:    return (SimplifyExpression(aExp::bitOr(op1, op2)));
case llvm::Instruction::Xor:   return (SimplifyExpression(aExp::bitXor(op1, op2)));
case llvm::Instruction::Shl:   return (SimplifyExpression(aExp::ShiftL(op1, op2)));
case llvm::Instruction::LShr:  return (SimplifyExpression(aExp::lShiftR(op1, op2)));
case llvm::Instruction::AShr:  return (SimplifyExpression(aExp::aShiftR(op1, op2)));
                        assert(0 && "Nepodrzana operacija jos uvek!!!");

case llvm::Instruction::BitCast:  return op1; //FIXME
case llvm::Instruction::IntToPtr: return IntToPtr(op1);
case llvm::Instruction::PtrToInt: return PtrToInt(op1);

case llvm::Instruction::GetElementPtr: {
std::cout << std::endl << std::endl << "evalConstantExpr ---- GetElementPtr" <<     std::endl;   

  //FIXME ovo je krpljevina i jako je sumnjivo
  if(llvm::dyn_cast<llvm::GlobalValue>(ce->getOperand(0)))
        op1 = ExpGlobalAddress(GetOperandName(ce->getOperand(0)));

        aExp base = op1; 
        aExp add = ExpNumZeroInt;

        for (llvm::gep_type_iterator ii = gep_type_begin(ce), ie = llvm::gep_type_end(ce); ii != ie; ++ii) 
        {
        
          aExp addend = ExpNumZeroInt; 
          
          if (llvm::StructType *st = llvm::dyn_cast<llvm::StructType>(*ii)) {
          const llvm::StructLayout *sl = GetParentBlock()->GetParentModule()->GetTargetData()->getStructLayout(st);
          const llvm::ConstantInt *ci = llvm::cast<llvm::ConstantInt>(ii.getOperand());
          addend = ExpNum1((int)sl->getElementOffset((unsigned)ci->getZExtValue()), fint_type); 
          } else {
          const llvm::SequentialType *sty = llvm::cast<llvm::SequentialType>(*ii);
        aExp e = evalConstant(llvm::cast<llvm::Constant>(ii.getOperand()));
        aExp index = ToInt(e, e.getIntWidth());
          unsigned elementSize = GetParentBlock()->GetParentModule()->GetTargetData()->getTypeStoreSize(sty->getElementType());

          addend = SimplifyExpression(aExp::mul(index, ExpNum1(elementSize, fint_type)));
          }
        //ovo je sumnjivo
        add = SimplifyExpression(aExp::add(add, addend));
        }

        base = ExpGepi(base, add);
//std::cout << "EvalConstantExpr ---------------------------------------" << std::endl;
//base.Print(&SMTF, std::cout);
//std::cout << std::endl << "EvalConstantExpr ---------------------------------------" << std::endl;
    return base;
}


case llvm::Instruction::ICmp: {
switch(ce->getPredicate()) {
default: assert(0 && "unhandled ICmp predicate");
case llvm::ICmpInst::ICMP_EQ:  return aExp::Equality(op1, op2);
case llvm::ICmpInst::ICMP_NE:  return aExp::Disequality(op1, op2);
case llvm::ICmpInst::ICMP_UGT: return aExp::ugt(op1, op2);
case llvm::ICmpInst::ICMP_UGE: return aExp::uge(op1, op2);
case llvm::ICmpInst::ICMP_ULT: return aExp::ult(op1, op2);
case llvm::ICmpInst::ICMP_ULE: return aExp::ule(op1, op2);
case llvm::ICmpInst::ICMP_SGT: return aExp::sgt(op1, op2);
case llvm::ICmpInst::ICMP_SGE: return aExp::sge(op1, op2);
case llvm::ICmpInst::ICMP_SLT: return aExp::slt(op1, op2);
case llvm::ICmpInst::ICMP_SLE: return aExp::sle(op1, op2);
}
}

case llvm::Instruction::Select:
assert(0 && "unhandled Select instruction!!!");
return op2;

case llvm::Instruction::FAdd:
case llvm::Instruction::FSub:
case llvm::Instruction::FMul:
case llvm::Instruction::FDiv:
case llvm::Instruction::FRem:
case llvm::Instruction::FPTrunc:
case llvm::Instruction::FPExt:
case llvm::Instruction::UIToFP:
case llvm::Instruction::SIToFP:
case llvm::Instruction::FPToUI:
case llvm::Instruction::FPToSI:
case llvm::Instruction::FCmp: 
//        std::cout << "FIXME: fxxx in constant expression! " << std::endl;
assert(0 && "floating point ConstantExprs unsupported");
}

//ovo je cisto da bi funkcija nesto vratila da ne bi bilo warning-a
//        std::cout << "ExpZeroExpZeroExpZero111111111111111" << std::endl;
return op1;

}

//FIXME ovo treba sve zakrpiti
aExp LState::evalConstant(llvm::Constant *c) {

  //inicijalizacija da ne bi pucalo
  aExp e = ExpNumZeroInt;
  if(c == NULL) return e;
 //  llvm::outs() << "evalConstant " << *c << '\n'; 
//_Store.Print(std::cout);
  if (llvm::ConstantExpr *ce = llvm::dyn_cast<llvm::ConstantExpr>(c)) {
      return evalConstantExpr(ce);
    } else {
      if (const llvm::ConstantInt *ci = llvm::dyn_cast<llvm::ConstantInt>(c)) {
        return ExpNum1(ci->getValue().toString(10,true), GetIntType(c->getType()));
      } else if (const llvm::ConstantFP *cf = llvm::dyn_cast<llvm::ConstantFP>(c)) {      
        switch(cf->getType()->getTypeID()) {
        case llvm::Type::FloatTyID: {
          float f = cf->getValueAPF().convertToFloat();
          //FIXME tip
//          std::cout << "float evalConstant " << *c << std::endl; 
          return ExpNum1(f, argo::sInt32Type);
        }
        case llvm::Type::DoubleTyID: {
          double d = cf->getValueAPF().convertToDouble();
          //FIXME tip
 //         std::cout << "double evalConstant " << *c << std::endl; 
          return ExpNum1(d, argo::sInt64Type);
//          return ExpNum1(d, argo::sInt32Type);
        }       
        case llvm::Type::X86_FP80TyID: {
          llvm::APFloat apf = cf->getValueAPF();
          bool ignored;
          llvm::APFloat::opStatus r = apf.convert(llvm::APFloat::IEEEdouble, 
                                            llvm::APFloat::rmNearestTiesToAway,
                                            &ignored);
          (void) r;
          double d = apf.convertToDouble();
          //FIXME tip
//          std::cout << "X86_FP80TyID evalConstant " << *c << std::endl; 
          return ExpNum1(d, argo::sInt64Type);
//          return ExpNum1(d, argo::sInt32Type);
        }
        default:                
         llvm::errs() << "Constant of type " << *(cf->getType()) 
                      << " not supported\n";            
          assert(0 && "Arbitrary bit width floating point constants unsupported");
        }
      } else if (llvm::GlobalValue *gv = llvm::dyn_cast<llvm::GlobalValue>(c)) {
        {

  //FIXME ovde je svasta ispusteno!!!
//        std::cout << "GlobalValue " << *gv;
        if(llvm::GlobalVariable *gvar = llvm::dyn_cast<llvm::GlobalVariable>(gv))
        {  
          if(gvar->isConstant())       
          {
            if(gvar->hasInitializer() && gvar->hasDefinitiveInitializer())       
              return evalConstant(gvar->getInitializer());
          }
        //FIXME Tip od select-a je ispusten
        //FIXME mozda ako vrati exp::top da onda vrati neki broj - zbog slaganja izraza
        return GetGlobalValue(GetOperandName(gv));
        }

//FIXME
return ExpNumZeroInt;
        }
      } else if (llvm::isa<llvm::ConstantPointerNull>(c)) {
          return ExpNumZeroPtr;       
      } else if (llvm::isa<llvm::UndefValue>(c)) {
//mozda treba neko globalno ime a ne lokalno, nisam sigurna, 
//mozda je dobro ovako, tip je svakako los
return ExpVar(GetNameOfNextVariable(), argo::sInt32Type, false );
      } else {
        // Constant{AggregateZero,Array,Struct,Vector}
        assert(0 && "invalid argument to evalConstant()");
      }
    }
  return e;
}



/*
    } else if (isa<UndefValue>(c) || isa<ConstantAggregateZero>(c)) {
      return ConstantExpr::create(0, getWidthForLLVMType(c->getType()));
#if LLVM_VERSION_CODE >= LLVM_VERSION(3, 1)
    } else if (const ConstantDataSequential *cds =
                 dyn_cast<ConstantDataSequential>(c)) {
      std::vector<ref<Expr> > kids;
      for (unsigned i = 0, e = cds->getNumElements(); i != e; ++i) {
        ref<Expr> kid = evalConstant(cds->getElementAsConstant(i));
        kids.push_back(kid);
      }
      ref<Expr> res = ConcatExpr::createN(kids.size(), kids.data());
      return cast<ConstantExpr>(res);
#endif
    } else if (const ConstantStruct *cs = dyn_cast<ConstantStruct>(c)) {
      const StructLayout *sl = kmodule->targetData->getStructLayout(cs->getType());
      llvm::SmallVector<ref<Expr>, 4> kids;
      for (unsigned i = cs->getNumOperands(); i != 0; --i) {
        unsigned op = i-1;
        ref<Expr> kid = evalConstant(cs->getOperand(op));

        uint64_t thisOffset = sl->getElementOffsetInBits(op),
                 nextOffset = (op == cs->getNumOperands() - 1)
                              ? sl->getSizeInBits()
                              : sl->getElementOffsetInBits(op+1);
        if (nextOffset-thisOffset > kid->getWidth()) {
          uint64_t paddingWidth = nextOffset-thisOffset-kid->getWidth();
          kids.push_back(ConstantExpr::create(0, paddingWidth));
        }

        kids.push_back(kid);
      }
      ref<Expr> res = ConcatExpr::createN(kids.size(), kids.data());
      return cast<ConstantExpr>(res);
    } else {
      // Constant{Array,Vector}
      assert(0 && "invalid argument to evalConstant()");
    }
  }
*/




//id1 je tekuci a id2 je prethodnik
//ovo bi mozda trebalo delegirati da se uradi u store-u
aExp LState::ConnectVariables(unsigned id1, unsigned id2) const
{
  vaExp exps;
  std::map<std::string, LVariableInfo>::const_iterator iter = _Store.GetStore().begin(), itere = _Store.GetStore().end();
  for(;iter!= itere; iter++)
  {
    //Prespajaju se samo relevantne promenljive
    if(!iter->second.IsRelevant()) { continue;  }
    //FIXME zasto se ovo stampa 4 puta za vv1
    if(!iter->second.Connect()) { continue;}

    std::string s = iter->first;
    const llvm::Type* t = iter->second.Type();
    const aExp* e;
    if(((e = iter->second.Value()) != NULL))
      {
        aExp v1 = MakeExp(VALUE, GetParentBlock()->GetFunctionName(), s, id1, 0, t);
        aExp v2 = MakeExp(VALUE, GetParentBlock()->GetFunctionName(), s, id2, 1, t);
        exps.push_back(aExp::Equality(v1,v2));
      }
  }

  return MakeANDFromExpressions(exps); 
}

static bool IsComparison(const aExp* e)
{
  if(e->IsEquality())    return true;
  if(e->IsDisequality()) return true;
  if(e->isUlt())         return true;
  if(e->isUle())         return true;
  if(e->isUgt())         return true;
  if(e->isUge())         return true;
  if(e->isSlt())         return true;
  if(e->isSle())         return true;
  if(e->isSgt())         return true;
  if(e->isSge())         return true;

  return false;
}

aExp LState::TransformationFormula() const
{
  vaExp exps;

  std::map<std::string, LVariableInfo>::const_iterator iter, itere;
  iter = _Store.GetStore().begin(); 
  itere = _Store.GetStore().end();
  for( ; iter!= itere; iter++)
  {
    const aExp* e = iter->second.Value(); 
    std::string s = iter->first;
    const llvm::Type* t = iter->second.Type();
    if(iter->second.IncludeInTransformation())  
    if(e != NULL)
//      if(!IsComparison(e)) 
      {
      aExp v1 = MakeExp(VALUE, GetParentBlock()->GetFunctionName(), s, GetParentBlock()->Id(), 1, t);
      exps.push_back(aExp::Equality(v1,*e));
      }
  }
  return MakeANDFromExpressions(exps);
}


void LState::AddStore(const std::string& variable, const LVariableInfo& fvi)
{

bool relevant = fvi.IsRelevant();
const llvm::Type* type = fvi.Type();
aExp value;

if(fvi.Value() == NULL) value = MakeExp(VALUE, GetParentBlock()->GetFunctionName(), variable, GetParentBlock()->Id(), 0, type);
else value = *(fvi.Value());

_Store.Write(variable, new aExp(value), type, relevant);
}

void LState::AddGlobalInitializations()
{ 
for (llvm::Module::global_iterator I = GetParentBlock()->GetParentModule()->GetLLVMModule()->global_begin(), E = GetParentBlock()->GetParentModule()->GetLLVMModule()->global_end();
I != E; ++I) {
if(llvm::GlobalVariable* gv = llvm::dyn_cast<llvm::GlobalVariable>(I))
{ 
    if(isRelevantGlobalVariable(gv))
    {
        if(gv->hasInitializer())     
        {
            aExp value = evalConstant(gv->getInitializer());
            if(GetIntType(gv->getType())!=fint_type) 
                value = SetWidth(value, GetIntType(gv->getType()),fint_type);
            std::string name = GlobalName(GetOperandName(gv));

            if(_Store.IsDefined(name))
                _Store.ChangeValue(name, new aExp(value));
            else
            {
            aExp mem = *GetValue(MEMORY);
            aExp index = ExpGlobalAddress(GetOperandName(gv));

            //FIXME kada je globalni niz u pitanju, on postavi samo prvu nulu, ne postavi sve nule
            MemoryStore(mem, index, value);
            }
        }

      }
}
}
}

void LState::ResetMemory()    
{
  aExp newmem = ExpArray(GetNameOfNextVariable());
    _Store.ChangeValue(MEMORY, new aExp(newmem));
}

//mozda bi trebalo reorganizovati store, da ima obican i da ima globalni deo
//pa onda ovo lepo sve u store prebaciti
aExp LState::GetGlobalValue(const std::string& name) 
{
std::string s = GlobalName(name);
if(_Store.IsDefined(s))
    return *_Store.GetValue(s);
else
{
    //FIXME ovde treba podesiti tip
    if(!TrackPointers) return ExpVar(GetNameOfNextVariable(), argo::sInt32Type, false);

    aExp mem = *GetValue(MEMORY);
    aExp index = ExpGlobalAddress(name);
    return aExp::select(mem, index);
}
}

}//end of namespace

