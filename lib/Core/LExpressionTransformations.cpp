//===-- LExpressionTransformation.cpp
//--------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "expression/expressions/Expression.h"
#include "expression/output/SMTFormater.h"

#include "lav/Internal/LExpressionTransformation.h"
#include "lav/Internal/LCommon.h"

namespace lav {
static argo::SMTFormater SMTF;

argo::Expression SetWidth(const argo::Expression &e, unsigned width_old,
                          unsigned width_new) {
  if (width_old == width_new)
    return e;
  else if (width_old < width_new) {
    if (e.IsNumeral())
      return ExpNum1(e.GetValue(), GetIntType(width_new));
    else
      return argo::Expression::sext(e, width_new);
  } else {
    if (e.IsNumeral())
      return ExpNum1(e.GetValue(), GetIntType(width_new));
    else
      return argo::Expression::extract(e, 0, width_new - 1);
  }
}

argo::Expression ToInt(const argo::Expression &e, unsigned width) {
  static unsigned int width_int = GetBitWidth(fint_type);
  return SetWidth(e, width, width_int);
}

argo::Expression ToPointer(const argo::Expression &e, unsigned width) {
  static unsigned int width_pointer = GetBitWidth(fpointer_type);
  return SetWidth(e, width, width_pointer);
}

argo::Expression PtrToInt(const argo::Expression &e) {
  static unsigned int width_int = GetBitWidth(fint_type);
  static unsigned int width_pointer = GetBitWidth(fpointer_type);

  return SetWidth(e, width_pointer, width_int);
}

argo::Expression IntToPtr(const argo::Expression &e) {
  static unsigned int width_int = GetBitWidth(fint_type);
  static unsigned int width_pointer = GetBitWidth(fpointer_type);
  return SetWidth(e, width_int, width_pointer);
}

argo::Expression
MakeANDFromExpressions(const std::vector<argo::Expression> &exps) {
  if (exps.size() > 1)
    return argo::Expression::AND(exps);
  else if (exps.size() == 1)
    return exps[0];
  else
    return argo::Expression::TOP();
}

bool IsGlobalVariable(const argo::Expression &e) {
  if ((!e.IsVariable()) && (!e.IsFormulaVariable()))
    return false;
  std::string name = e.GetName(), global = std::string(GLOBAL);
  if ((name.size() <= global.size()))
    return false;
  for (unsigned i = 0; i < global.size(); i++)
    if (name[i] != global[i])
      return false;
  return true;
}

argo::Expression RenameExpressionVariables(const argo::Expression &e,
                                           const int& Context,
                                           const std::string &FunctionName) {
  if (Context == 0)
    return e;
  if (e.IsTOP() || e.IsBOT() || e.IsNumeral())
    return e;

  if (e.IsVariable()) {
    if (IsGlobalVariable(e) || isGlobalAddress(e.GetName()))
      return e;
    else
      return ExpVar(AddContext(e.GetName(), Context, FunctionName),
                    e.getIntType(), e.isRelevant());
  }
  if (e.IsFormulaVariable()) {
    return ExpFVar(AddContext(e.GetName(), Context, FunctionName));
  }

  //Ovo je pitanje da li je potrebno
  if (e.GetArity() == 0)
    return e;
  //Apstrahovanje svih operanada
  std::vector<argo::Expression> operands;
  for (unsigned i = 0; i < e.GetArity(); i++)
    operands.push_back(RenameExpressionVariables(e[i], Context, FunctionName));
  bool b;
  return MakeExpression(e, operands, b);
}


argo::Expression RenameExpressionVariables(const argo::Expression &e,
                                           const std::string &Context) {
  if (e.IsTOP() || e.IsBOT() || e.IsNumeral())
    return e;

  if (e.IsVariable()) {
    if (IsGlobalVariable(e) || isGlobalAddress(e.GetName()))
      return e;
    else
      return ExpVar(e.GetName()+Context,
                    e.getIntType(), e.isRelevant());
  }
  if (e.IsFormulaVariable()) {
    return ExpFVar(e.GetName()+Context);
  }

  //Ovo je pitanje da li je potrebno
  if (e.GetArity() == 0)
    return e;
  //Apstrahovanje svih operanada
  std::vector<argo::Expression> operands;
  for (unsigned i = 0; i < e.GetArity(); i++)
    operands.push_back(RenameExpressionVariables(e[i], Context));
  bool b;
  return MakeExpression(e, operands, b);
}


argo::Expression RenameExpressionVariables(const argo::Expression &e,
                                           const std::string &Context, int c) {
  if (c == 0)
    return e;
    return RenameExpressionVariables(e, Context);
}

argo::Expression MakeExpression(const argo::Expression &e,
                                const std::vector<argo::Expression> &operands,
                                bool &found) {
  found = true;
  if (e.isAdd())
    return argo::Expression::add(operands[0], operands[1]);
  if (e.isSub())
    return argo::Expression::sub(operands[0], operands[1]);
  if (e.isMul())
    return argo::Expression::mul(operands[0], operands[1]);
  if (e.isSdiv())
    return argo::Expression::sdiv(operands[0], operands[1]);
  if (e.isUdiv())
    return argo::Expression::udiv(operands[0], operands[1]);
  if (e.isSrem())
    return argo::Expression::srem(operands[0], operands[1]);
  if (e.isUrem())
    return argo::Expression::urem(operands[0], operands[1]);
  if (e.isUlt())
    return argo::Expression::ult(operands[0], operands[1]);
  if (e.isUle())
    return argo::Expression::ule(operands[0], operands[1]);
  if (e.isUgt())
    return argo::Expression::ugt(operands[0], operands[1]);
  if (e.isUge())
    return argo::Expression::uge(operands[0], operands[1]);
  if (e.isSlt())
    return argo::Expression::slt(operands[0], operands[1]);
  if (e.isSle())
    return argo::Expression::sle(operands[0], operands[1]);
  if (e.isSgt())
    return argo::Expression::sgt(operands[0], operands[1]);
  if (e.isSge())
    return argo::Expression::sge(operands[0], operands[1]);
  if (e.isBitAnd())
    return argo::Expression::bitAnd(operands[0], operands[1]);
  if (e.isBitOr())
    return argo::Expression::bitOr(operands[0], operands[1]);
  if (e.isBitXor())
    return argo::Expression::bitXor(operands[0], operands[1]);
  if (e.isShiftL())
    return argo::Expression::ShiftL(operands[0], operands[1]);
  if (e.islShiftR())
    return argo::Expression::lShiftR(operands[0], operands[1]);
  if (e.isaShiftR())
    return argo::Expression::aShiftR(operands[0], operands[1]);
  if (e.isSext())
    return argo::Expression::sext(operands[0], operands[1].GetValue().get_si());
  if (e.isZext())
    return argo::Expression::zext(operands[0], operands[1].GetValue().get_si());
  if (e.isExtract())
    return argo::Expression::extract(operands[0],
                                     operands[1].GetValue().get_si(),
                                     operands[2].GetValue().get_si());
  if (e.IsAND())
    return argo::Expression::AND(operands);
  if (e.IsOR())
    return argo::Expression::OR(operands);
  if (e.IsNOT())
    return argo::Expression::NOT(operands[0]);
  if (e.IsIMPL())
    return argo::Expression::IMPL(operands[0], operands[1]);
  if (e.IsIFF())
    return argo::Expression::IFF(operands[0], operands[1]);
  if (e.IsXOR())
    return argo::Expression::NOT(
               argo::Expression::IFF(operands[0], operands[1])); //argo nema xor
  if (e.IsEquality())
    return argo::Expression::Equality(operands[0], operands[1]);
  if (e.IsDisequality())
    return argo::Expression::NOT(
        argo::Expression::Equality(operands[0], operands[1]));
  if (e.isLeft())
    return ExpLeft(operands[0]);
  if (e.isRight())
    return ExpRight(operands[0]);
  if (e.isArray())
    return ExpArray(operands[0].GetName());
  if (e.isStore())
    return argo::Expression::store(operands[0], operands[1], operands[2]);
  if (e.isSelect())
    return argo::Expression::select(operands[0], operands[1]);
  if (e.IsITE())
    return argo::Expression::IfThenElse(operands[0], operands[1], operands[2]);

  //TODO ovo nije podrzano
  //if (e.IsPredicate())    return argo::Expression::Predicate(e.GetName(),
  //operands);

  if (e.IsFunction())
    return aExp::Function(e.GetName(), operands, e.getIntType());

  found = false;
  return e;
}

void SetUnion(std::set<argo::Expression> &set1,
              const std::set<argo::Expression> &set2) {
  std::set<argo::Expression>::const_iterator it = set2.begin(),
                                             ite = set2.end();
  for (; it != ite; it++)
    set1.insert(*it);
}

void CopySetIntoVector(const std::set<argo::Expression> &set,
                       std::vector<argo::Expression> &vector) {
  std::set<argo::Expression>::iterator it = set.begin(), ite = set.end();
  for (; it != ite; it++)
    vector.push_back(*it);
}

void AddEqs(const argo::Expression &a, std::set<argo::Expression> &eq,
            argo::Expression &result) {
  std::vector<argo::Expression> eqs;
  CopySetIntoVector(eq, eqs);
  AddEqs(a, eqs, result);
}

argo::Expression AddEqs(const argo::Expression &a,
                        std::set<argo::Expression> &eq) {
  argo::Expression result;
  AddEqs(a, eq, result);
  return result;
}

void AddEqs(const argo::Expression &a, std::vector<argo::Expression> &eqs,
            argo::Expression &result) {
  if (eqs.size() == 0)
    result = a;
  if (eqs.size() == 1)
    result = argo::Expression::AND(a, eqs[0]);
  if (eqs.size() > 1) {
    eqs.push_back(a);
    result = argo::Expression::AND(eqs);
  }
}

argo::Expression AddEqs(const argo::Expression &a,
                        std::vector<argo::Expression> &eqs) {
  argo::Expression result;

  if (eqs.size() == 0)
    result = a;
  if (eqs.size() == 1)
    result = argo::Expression::AND(a, eqs[0]);
  if (eqs.size() > 1) {
    eqs.push_back(a);
    result = argo::Expression::AND(eqs);
  }

  return result;
}

} //end of namespace
