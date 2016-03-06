//===-- FCommon.cpp --------------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <string>

#include "lav/Internal/LCommon.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/Support/CallSite.h"
#include "llvm/Support/CommandLine.h"

#include "expression/output/SMTFormater.h"

#include "expression/expressions/NumeralTypes.h"


using namespace llvm;


namespace lav
{
static argo::SMTFormater SMTF;

argo::Expression EvaluateArithmetic(const argo::Expression& e, const argo::Expression& e1, const argo::Expression& e2)
{
    if(e.isAdd())
                return ExpNum1(e1.GetValue() + e2.GetValue(), e1.getIntType());
    if(e.isSub())
                return ExpNum1(e1.GetValue() - e2.GetValue(), e1.getIntType());
    if(e.isMul())
                return ExpNum1(e1.GetValue() * e2.GetValue(), e1.getIntType());

    if(e.isUdiv() || e.isSdiv()) {
                if((e2.GetValue() != 0))
                {
                if((e1.GetValue() == 0)) return ExpNum1(e1.GetValue(), e1.getIntType());
                else      
                      return  ExpNum1(e1.GetValue() / e2.GetValue(), e1.getIntType());
                }
                //ovo je abstrakcija deljenja nulom
                  else return ExpVar(GetNextVariable(), e1.getIntType(), false);
                }
    if(e.isUrem() || e.isSrem() ) {
                if((e2.GetValue() != 0))
                {
                if((e1.GetValue() == 0)) return ExpNum1(e1.GetValue(), e1.getIntType());
                else      
                      return ExpNum1(e1.GetValue() % e2.GetValue(), e1.getIntType());            
                }
                //ovo je abstrakcija deljenja nulom
                else return ExpVar(GetNextVariable(), e1.getIntType(), false);
                }
    
    if(e.isBitAnd())
                return ExpNum1(e1.GetValue() & e2.GetValue(), e1.getIntType());
    if(e.isBitOr())
                return ExpNum1(e1.GetValue() | e2.GetValue(), e1.getIntType());
    if(e.isBitXor())
                return ExpNum1(e1.GetValue() ^ e2.GetValue(), e1.getIntType());
    //ovo je krpljevina
    if(e.isShiftL())
                return ExpNum1(e1.GetValue().get_si() << e2.GetValue().get_si(), e1.getIntType());
    if(e.islShiftR() )
                return ExpNum1(unsigned(e1.GetValue().get_si()) >> (unsigned)e2.GetValue().get_si(), e1.getIntType());
    if(e.isaShiftR())
                return ExpNum1(e1.GetValue().get_si() >> e2.GetValue().get_si(), e1.getIntType());

    return e;

}

//zbog ovoga mogu da se propuste greske prekoracnja!
argo::Expression SimplifyRelation(const argo::Expression& e)
{
    argo::Expression arg1 = e[0];
    argo::Expression arg2 = e[1];
    argo::Expression e1, e2;
    if(arg2.IsNumeral() && arg1.isAdd() && arg1[1].IsNumeral()) {
      e1 = arg1[0];
      e2 = ExpNum1(arg2.GetValue().get_si() - arg1[1].GetValue().get_si(), arg2.getIntType());
    } else if(arg2.IsNumeral() && arg1.isSub() && arg1[1].IsNumeral()) {
      e1 = arg1[0];
      e2 = ExpNum1(arg2.GetValue().get_si() + arg1[1].GetValue().get_si(), arg2.getIntType());
    } else if(arg1.IsNumeral() && arg2.isAdd() && arg2[1].IsNumeral()) {
      e1 = arg2[0];
      e2 = ExpNum1(arg1.GetValue().get_si() - arg2[1].GetValue().get_si(), arg1.getIntType());
    } else if(arg1.IsNumeral() && arg2.isSub() && arg2[1].IsNumeral()) {
      e1 = arg2[0];
      e2 = ExpNum1(arg1.GetValue().get_si() + arg2[1].GetValue().get_si(), arg1.getIntType());
    } else if(arg1.IsNumeral() && arg2.isSub() && arg2[0].IsNumeral()) {
      e1 = arg2[1];
      e2 = ExpNum1(arg2[0].GetValue().get_si() - arg1.GetValue().get_si(), arg1.getIntType());
    } else if(arg2.IsNumeral() && arg1.isSub() && arg1[0].IsNumeral()) {
      e1 = ExpNum1(arg1[0].GetValue().get_si() - arg2.GetValue().get_si(), arg2.getIntType());
      e2 = arg1[1];
    } else return e;

    if(e.isUlt())  return argo::Expression::ult(e1,e2);
    if(e.isUle())  return argo::Expression::ule(e1,e2);
    if(e.isUgt())  return argo::Expression::ugt(e1,e2);
    if(e.isUge())  return argo::Expression::uge(e1,e2);
    if(e.isSlt())  return argo::Expression::slt(e1,e2);
    if(e.isSle())  return argo::Expression::sle(e1,e2);
    if(e.isSgt())  return argo::Expression::sgt(e1,e2);
    if(e.isSge())  return argo::Expression::sge(e1,e2);
    if(e.IsEquality()) return argo::Expression::Equality(e1,e2);
    if(e.IsDisequality()) return argo::Expression::Disequality(e1,e2);
return e;

}

//FIXME ovo ce da napravi rusvaj za signed i unsigned
argo::Expression EvaluateRelation(const argo::Expression& e, const argo::Expression& e1, const argo::Expression& e2)
{
    if(e.isUlt()) 
    {   
    if(e1.GetValue()<e2.GetValue()) return argo::Expression::TOP();
    else return argo::Expression::BOT();
    }
    if(e.isUle()) 
    {   
    if(e1.GetValue()<=e2.GetValue()) return argo::Expression::TOP();
    else return argo::Expression::BOT();
    }
    if(e.isUgt()) 
    {   
    if(e1.GetValue()>e2.GetValue()) return argo::Expression::TOP();
    else return argo::Expression::BOT();
    }
    if(e.isUge())
    {   
    if(e1.GetValue()>=e2.GetValue()) return argo::Expression::TOP();
    else return argo::Expression::BOT();
    }
    if(e.isSlt()) 
    {  
    if(e1.GetValue().get_si()<e2.GetValue().get_si()) return argo::Expression::TOP();
    else return argo::Expression::BOT();
    }
    if(e.isSle()) 
    {   
    if(e1.GetValue().get_si()<=e2.GetValue().get_si()) return argo::Expression::TOP();
    else return argo::Expression::BOT();
    }
    if(e.isSgt()) 
    {   
    if(e1.GetValue().get_si()>e2.GetValue().get_si()) return argo::Expression::TOP();
    else return argo::Expression::BOT();
    }
    if(e.isSge())
    {   
    if(e1.GetValue().get_si()>=e2.GetValue().get_si()) return argo::Expression::TOP();
    else return argo::Expression::BOT();
    }
    if(e.IsEquality())
    {   
    if(e1.GetValue()==e2.GetValue()) return argo::Expression::TOP();
    else return argo::Expression::BOT();
    }
    if(e.IsDisequality())
    {   
    if(e1.GetValue()!=e2.GetValue()) return argo::Expression::TOP();
    else return argo::Expression::BOT();
    }

    return e;
}

//TODO evaluirati i ostale operacije
argo::Expression Evaluate(const argo::Expression& e, const argo::Expression& e1, const argo::Expression& e2)
{
    argo::Expression result = e;
    if(e.isAdd() || e.isSub() || e.isMul() || 
        e.isUrem() || e.isSrem() || 
        e.isUdiv() || e.isSdiv() || 
        e.isBitAnd() || 
        e.isBitOr() || e.isBitXor()
        || e.isShiftL() || e.islShiftR() || e.isaShiftR())
    return EvaluateArithmetic(e, e1, e2);
    if(e.isUlt() || e.isUle()  || e.isUgt()  || e.isUge()  
        || e.isSlt()  || e.isSle()  || e.isSgt()  || e.isSge() 
        || e.IsEquality() || e.IsDisequality())
    return EvaluateRelation(e, e1, e2); 

    return result;
}

argo::Expression SimplifyExpressionSelect(const argo::Expression& e)
{
  if(!e.isSelect()) return e;

  //memorija nije store ili indeks nije fiksan --- nema sta da se simplifikuje
  if(!e[0].isStore() || VariableIndex(e[1])) return e;

  //ako je memorija store i ako je indeks adresa+numeral
  argo::Expression store = e[0];
  argo::Expression index = e[1];
  bool found = false;
  argo::Expression sindex;

  while(1)
  {
   if(!store.isStore()) break;
   sindex = store[1];
   if(VariableIndex(sindex)) break;
   if(sindex == index) {found = true; break;}
   store = store[0];
  }
  if(!found) return e;
  else
  {
    return store[2];
  }

}

argo::Expression SimplifyExpressionStore(const argo::Expression& e)
{
  if(!e.isStore()) return e;

  //memorija nije store ili indeks nije fiksan --- nema sta da se simplifikuje
  if(!e[0].isStore() || VariableIndex(e[1])) return e;

  //ako je memorija store i ako je indeks adresa+numeral
  argo::Expression store = e[0];
  argo::Expression index = e[1];
  argo::Expression value = e[2];

  std::vector<argo::Expression> vindex;
  std::vector<argo::Expression> vvalue;
  bool found = false;
  argo::Expression sindex, svalue;
  while(1)
  {
   if(!store.isStore()) break;
   sindex = store[1];
   svalue = store[2];
   if(VariableIndex(sindex)) break;
   if(sindex == index) {store = store[0]; found = true; break;}
   vindex.push_back(sindex);
   vvalue.push_back(svalue);
   store = store[0];
  }

  if(!found) return e;
  else
  {
    argo::Expression a = store;
    for(unsigned i=0; i<vindex.size();i++)
      a = argo::Expression::store(a, vindex[i], vvalue[i]);
    return argo::Expression::store(a, index, value);
  }

}

bool SimplifyFormulaOR(argo::Expression& e)
{
unsigned i;

for(i = 0; i < e.GetArity(); i++)
{
    if(e[i].IsTOP())
    {
    e = argo::Expression::TOP();
    return true;
    }
}
return false;
}

void SimplifyFormulaAND(std::vector<argo::Expression>& expressions)
{
unsigned i;

for(i = 0; i < expressions.size(); i++)
{
    if(expressions[i].IsBOT())
    {
        expressions.clear();
        expressions.push_back(argo::Expression::BOT());
    }

   if(expressions[i].IsOR())
    {bool b = SimplifyFormulaOR(expressions[i]); if(b) i--;}
   
}
}


argo::Expression OneNumeralAdd(const argo::Expression& e)
{
    argo::Expression arg1 = e[0];
    argo::Expression arg2 = e[1];

    if(arg2.IsNumeral())
    {
        if(arg1.isAdd() || arg1.isSub())
        {
            //a+3 + 5
            if(arg1[1].IsNumeral() && arg1.isAdd())
            {
            argo::Expression sum = argo::Expression::Numeral(arg1[1].GetValue() + arg2.GetValue(), arg1[1].getIntType());
            if(sum.GetValue() == 0) return arg1[0];
            return argo::Expression::add(arg1[0], sum);
            }	

            //a-3 + 5
            if(arg1[1].IsNumeral() && arg1.isSub())
            {
            argo::Expression sub = argo::Expression::Numeral(- arg1[1].GetValue() + arg2.GetValue(), arg1[1].getIntType());
            if(sub.GetValue() == 0) return arg1[0];
            return argo::Expression::add(arg1[0], sub);
            }	

            //3-a + 5
            if(arg1[0].IsNumeral() && arg1.isSub())
            {
            argo::Expression sub = argo::Expression::Numeral(arg1[0].GetValue() + arg2.GetValue(), arg1[0].getIntType());
            return argo::Expression::sub(sub, arg1[1]);
            }
        }
    return e;
    }

    if(arg1.IsNumeral())
    {
        if(arg2.isAdd() || arg2.isSub())
        {
            //5 + a+3
            if(arg2[1].IsNumeral() && arg2.isAdd())
            {
            argo::Expression sum = argo::Expression::Numeral(arg2[1].GetValue() + arg1.GetValue(), arg2[1].getIntType());
            if(sum.GetValue() == 0) return arg2[0];
            return argo::Expression::add(arg2[0], sum);
            }	

            //5 + a-3
            if(arg2[1].IsNumeral() && arg2.isSub())
            {
            argo::Expression sub = argo::Expression::Numeral(arg1.GetValue() - arg2[1].GetValue(), arg1.getIntType());
            if(sub.GetValue() == 0) return arg2[0];
            return argo::Expression::add(arg2[0], sub);
            }	

            //5 + 3-a
            if(arg2[0].IsNumeral() && arg2.isSub())
            {
            argo::Expression sum = argo::Expression::Numeral(arg2[0].GetValue() + arg1.GetValue(), arg1.getIntType());
            return argo::Expression::sub(sum, arg2[1]);
            }
        }
    //Pomeranje konstante na desnu stranu ako je 3+a da postane a+3
      return argo::Expression::add(arg2, arg1);
    }

return e;

}

argo::Expression OneNumeralSub(const argo::Expression& e)
{
    argo::Expression arg1 = e[0];
    argo::Expression arg2 = e[1];

    //nesto - 5
    if(arg2.IsNumeral())
        if(arg1.isAdd() || arg1.isSub())
        {
            //a+3 - 5
            if(arg1[1].IsNumeral() && arg1.isAdd())
            {
            argo::Expression sum = argo::Expression::Numeral(arg1[1].GetValue() - arg2.GetValue(), arg1[1].getIntType());
          return argo::Expression::add(arg1[0], sum);
            }	

            //a-3 - 5
            if(arg1[1].IsNumeral() && arg1.isSub())
            {
            argo::Expression sum = argo::Expression::Numeral(arg1[1].GetValue() + arg2.GetValue(), arg1[1].getIntType());
            if(sum.GetValue() == 0) return arg1[0];
          return argo::Expression::sub(arg1[0], sum);
            }	

            //3-a - 5
            if(arg1[0].IsNumeral() && arg1.isSub())
            {
            argo::Expression sub = argo::Expression::Numeral(arg1[0].GetValue() - arg2.GetValue(), arg1[0].getIntType());
          return argo::Expression::sub(sub, arg1[1]);
            }
        }

    //5 - nesto
    if(arg1.IsNumeral())
        if(arg2.isAdd() || arg2.isSub())
        {
            //5 - (a+3)
            if(arg2[1].IsNumeral() && arg2.isAdd())
            {
            argo::Expression sub = argo::Expression::Numeral(arg1.GetValue() - arg2[1].GetValue(), arg2[1].getIntType());
          return argo::Expression::sub(sub, arg2[0]);
            }	

            //5 - (a-3)
            if(arg2[1].IsNumeral() && arg2.isSub())
            {
            argo::Expression sum = argo::Expression::Numeral(arg1.GetValue() + arg2[1].GetValue(), arg1.getIntType());
          return argo::Expression::sub(sum, arg2[0]);
            }	

            //5 - (3-a)
            if(arg2[0].IsNumeral() && arg2.isSub())
            {
            argo::Expression sub = argo::Expression::Numeral( -arg2[0].GetValue() + arg1.GetValue(), arg1.getIntType());
          return argo::Expression::add(sub, arg2[1]);
            }
        }

return e;

}



argo::Expression TwoNumeralsBothAdd(const argo::Expression& e)
{
    argo::Expression arg1 = e[0];
    argo::Expression arg2 = e[1];

    if((arg1.isAdd() || arg1.isSub()) && (arg2.isAdd() || arg2.isSub()))
    {
        if((arg1[0].IsNumeral() || arg1[1].IsNumeral()) && (arg2[0].IsNumeral() || arg2[1].IsNumeral()) )
        {
            //a+3 + b+5
            if(arg1.isAdd() && arg2.isAdd())
            {
            argo::Expression sum = argo::Expression::add(arg1[0], arg2[0]);
            argo::Expression num = argo::Expression::Numeral(arg1[1].GetValue() + arg2[1].GetValue(), arg1[1].getIntType());
                if(num.GetValue() == 0) return sum;
            return argo::Expression::add(sum, num);                
            }
            

            //a+3 + b-5 ili a+3 + 5-b
            if(arg1.isAdd() && arg2.isSub())
            {
            //a+3 + b-5
            if(arg2[1].IsNumeral())
            {
            argo::Expression sum = argo::Expression::add(arg1[0], arg2[0]);
            argo::Expression num = argo::Expression::Numeral(arg1[1].GetValue() - arg2[1].GetValue(), arg1[1].getIntType());
                if(num.GetValue() == 0) return sum;
            return argo::Expression::add(sum, num);
            }                
            //a+3 + 5-b
            if(arg2[0].IsNumeral())
            {
            argo::Expression sub = argo::Expression::sub(arg1[0], arg2[1]);
            argo::Expression num = argo::Expression::Numeral(arg1[1].GetValue() + arg2[0].GetValue(), arg1[1].getIntType());
                if(num.GetValue() == 0) return sub;
            return argo::Expression::add(sub, num);
            }                
            }

            //a-3 + b+5 ili 3-a + b+5
            if(arg1.isSub() && arg2.isAdd())
            {
            //a-3 + b+5
            if(arg1[1].IsNumeral())
            {
            argo::Expression sum = argo::Expression::add(arg1[0], arg2[0]);
            argo::Expression num = argo::Expression::Numeral(arg2[1].GetValue() - arg1[1].GetValue(), arg1[1].getIntType());
                if(num.GetValue() == 0) return sum;
            return argo::Expression::add(sum, num);
            }                
            //3-a + b+5
            if(arg1[0].IsNumeral())
            {
            argo::Expression sub = argo::Expression::sub(arg2[0], arg1[1]);
            argo::Expression num = argo::Expression::Numeral(arg1[0].GetValue() + arg2[1].GetValue(), arg1[1].getIntType());
                if(num.GetValue() == 0) return sub;
            return argo::Expression::add(sub, num);
            }                
            }

            //a-3 + b-5 ili 3-a + b-5 ili 3-a + 5-b ili a-3 + 5-b
            if(arg1.isSub() && arg2.isSub())
            {
            //a-3 + b-5
            if(arg1[1].IsNumeral() && arg2[1].IsNumeral())
            {
            argo::Expression sum = argo::Expression::add(arg1[0], arg2[0]);
            argo::Expression num = argo::Expression::Numeral(arg1[1].GetValue() + arg2[1].GetValue(), arg1[1].getIntType());
                if(num.GetValue() == 0) return sum;
            return argo::Expression::sub(sum, num);
            }                
            //3-a + b-5
            if(arg1[0].IsNumeral() && arg2[1].IsNumeral())
            {
            argo::Expression sub = argo::Expression::sub(arg2[0], arg1[1]);
            argo::Expression num = argo::Expression::Numeral(arg1[0].GetValue() - arg2[1].GetValue(), arg1[1].getIntType());
                if(num.GetValue() == 0) return sub;
            return argo::Expression::add(sub, num);
            }                
            //3-a + 5-b
            if(arg1[0].IsNumeral() && arg2[0].IsNumeral())
            {
            argo::Expression sub = argo::Expression::add(arg2[1], arg1[1]);
            argo::Expression num = argo::Expression::Numeral(arg1[0].GetValue() + arg2[0].GetValue(), arg1[1].getIntType());
            return argo::Expression::sub(num, sub);
            }   
            //a-3 + 5-b
            if(arg1[1].IsNumeral() && arg2[0].IsNumeral())
            {
            argo::Expression sum = argo::Expression::sub(arg1[0], arg2[1]);
            argo::Expression num = argo::Expression::Numeral(arg2[0].GetValue() - arg1[1].GetValue(), arg1[1].getIntType());
                if(num.GetValue() == 0) return sum;
            return argo::Expression::add(sum, num);
            }                
         
            }
        }
    }
    return e;
}

argo::Expression GetConstantOut(const argo::Expression& e)
{
    argo::Expression arg1 = e[0];
    argo::Expression arg2 = e[1];

    //(a+3) + b => a+b + 3 || a + (b+5) => a+b +5
    if((arg1.isAdd() || arg1.isSub()) || (arg2.isAdd() || arg2.isSub()))
    {
        //a+3 + b
        if(arg1.isAdd() && arg1[1].IsNumeral())
        {
            argo::Expression sum = argo::Expression::add(arg1[0], arg2);
            return argo::Expression::add(sum, arg1[1]);            
        }
        //a + b+3
        if(arg2.isAdd() && arg2[1].IsNumeral())
        {
            argo::Expression sum = argo::Expression::add(arg1, arg2[0]);
            return argo::Expression::add(sum, arg2[1]);            
        }

        //a-3 + b
        if(arg1.isSub() && arg1[1].IsNumeral())
        {
            argo::Expression sum = argo::Expression::add(arg1[0], arg2);
            return argo::Expression::sub(sum, arg1[1]);            
        }

        //3-a + b => b-a +3
        if(arg1.isSub() && arg1[0].IsNumeral())
        {
            argo::Expression sub = argo::Expression::sub(arg2, arg1[1]);
            return argo::Expression::add(sub, arg1[0]);            
        }
        //a + b-3 => a+b -3
        if(arg2.isSub() && arg2[1].IsNumeral())
        {
            argo::Expression sum = argo::Expression::add(arg1, arg2[0]);
            return argo::Expression::sub(sum, arg2[1]);            
        }
        //a + 3-b => a-b +3
        if(arg2.isSub() && arg2[0].IsNumeral())
        {
            argo::Expression sub = argo::Expression::sub(arg1, arg2[1]);
            return argo::Expression::add(sub, arg2[0]);            
        }
    }
    return e;
}

argo::Expression TwoNumeralsAdd(const argo::Expression& e)
{
    argo::Expression arg1 = e[0];
    argo::Expression arg2 = e[1];

    if((arg1.isAdd() || arg1.isSub()) || (arg2.isAdd() || arg2.isSub()))
    {
    //a+3 + b+5 ili sa oduzmanjem razne kombinacije
    if((arg1.isAdd() || arg1.isSub()) && (arg2.isAdd() || arg2.isSub()))
        return TwoNumeralsBothAdd(e);

    //izbacivanje konstante napolje
    //(a+3) + b => a+b + 3 || a + (b+5) => a+b +5
    if((arg1.isAdd() || arg1.isSub()) || (arg2.isAdd() || arg2.isSub()))
        return GetConstantOut(e);
    }
    return e;
}

//Ako je sabiranje, onda gledamo sta su mu argumenti
//ako su oba numerali, sabiramo ih
//ako je jedan numeral, onda gledamo sta je drugi argument, i ako je drugi
//argument sabiranje ili oduzimanje sa numeralom, onda to sabiramo kako treba
//ako su oba argumenta sabiranja sa numeralom ili oduzimanja sa numeralom
//ona to izracunavamo kako treba
//ako je jedan argument sabiranje ili oduzimanje sa numeralom, odnda taj numeral
//izvlacimo da bude sa desne strane
//ako je samo sabiranje sa numeralom, onda podesavamo da je numeral sa desne strane
argo::Expression SimplifyAdd(const argo::Expression& e)
{
    if(e.isAdd())
    {
        argo::Expression arg1 = e[0];
        argo::Expression arg2 = e[1];

        if(arg1.IsNumeral() && arg2.IsNumeral())
            return argo::Expression::Numeral(arg1.GetValue() + arg2.GetValue(), arg1.getIntType());

        if(arg2.IsNumeral() && arg2.GetValue() == 0) return arg1;
        if(arg1.IsNumeral() && arg1.GetValue() == 0) return arg2;

        if(arg1.IsNumeral() || arg2.IsNumeral())
            return OneNumeralAdd(e);
        
        if((arg1.isAdd() || arg1.isSub()) || (arg2.isAdd() || arg2.isSub()))
            return TwoNumeralsAdd(e);

        //ako ne postoji nikakvo pojednostavljenje
        return e;
    }
return e;
}

argo::Expression SimplifySub(const argo::Expression& e)
{

if(e.isSub()) 
    {
        argo::Expression arg1 = e[0];
        argo::Expression arg2 = e[1];

        //ako su oba numerali, odmah ce ih oduzeti
        if(arg1.IsNumeral() && arg2.IsNumeral())
	        return argo::Expression::Numeral(arg1.GetValue() - arg2.GetValue(), arg1.getIntType());

        if(arg2.IsNumeral() && arg2.GetValue() == 0) return arg1;

        if(arg1.IsNumeral() || arg2.IsNumeral())
            return OneNumeralSub(e);

        //TODO nedostaju ostala pravila za oduzimanje
    }
    //ako ne postoji nikakvo pojednostavljenje
    return e;
}


argo::Expression SimplifyMul(const argo::Expression& e)
{
    if(e.isMul())
    {
        argo::Expression arg1 = e[0];
        argo::Expression arg2 = e[1];

        if(arg1.IsNumeral() && arg2.IsNumeral())
            return argo::Expression::Numeral(arg1.GetValue() * arg2.GetValue(), arg1.getIntType());

        if(arg2.IsNumeral() && arg2.GetValue() == 1) return arg1;
        if(arg1.IsNumeral() && arg1.GetValue() == 1) return arg2;

        //ako ne postoji nikakvo pojednostavljenje
        return e;
    }
return e;
}


argo::Expression SimplifyNot(const argo::Expression& e)
{
if(e[0].IsDisequality()) return aExp::Equality(e[0][0], e[0][1]);
if(e[0].IsEquality()) return aExp::Disequality(e[0][0], e[0][1]);
if(e[0].isUlt()) return aExp::uge(e[0][0], e[0][1]);
if(e[0].isUle()) return aExp::ugt(e[0][0], e[0][1]);
if(e[0].isUgt()) return aExp::ule(e[0][0], e[0][1]);
if(e[0].isUge()) return aExp::ult(e[0][0], e[0][1]);
if(e[0].isSlt()) return aExp::sge(e[0][0], e[0][1]);
if(e[0].isSle()) return aExp::sgt(e[0][0], e[0][1]);
if(e[0].isSgt()) return aExp::sle(e[0][0], e[0][1]);
if(e[0].isSge()) return aExp::slt(e[0][0], e[0][1]);
return e;
}

//TODO Dodati jos simplifikacija*/
argo::Expression SimplifyExpression(const argo::Expression& e)
{
  if(e.IsNumeral())
     return e;
  if(e.isStore())
    return SimplifyExpressionStore(e);
  if(e.isSelect())
    return SimplifyExpressionSelect(e);
  if(e.GetArity() == 2)
  {
    if(e[0].IsNumeral() && e[1].IsNumeral()) return Evaluate(e, e[0], e[1]);
    else if(e.isAdd()) return SimplifyAdd(e);
    else if(e.isSub()) return SimplifySub(e);
    else if(e.isMul()) return SimplifyMul(e);

/*    else if(e.isUlt() || e.isUle()  || e.isUgt()  || e.isUge()  
        || e.isSlt()  || e.isSle()  || e.isSgt()  || e.isSge() 
        || e.IsEquality() || e.IsDisequality()) return SimplifyRelation(e);*/
    else return e;
  }
  if(e.GetArity() == 1)
  {
  if(e.IsNOT()) return SimplifyNot(e);
  }
  return e;
}






} //end of namespace
