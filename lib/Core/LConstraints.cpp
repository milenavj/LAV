//===-- LConstraints.cpp --------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "lav/Internal/LExpressionTransformation.h"
#include "lav/Internal/LCommon.h"
#include "lav/Internal/LConstraints.h"
#include "expression/output/SMTFormater.h"
#include "expression/expressions/Expression.h"

extern llvm::cl::opt<bool> CheckPointers;

namespace lav {

static argo::SMTFormater SMTF;

void LConstraints::Print() const {
  for (unsigned k = 0; k < _Constraints.size(); k++) {
    _Constraints[k].Print(&SMTF, std::cout);
    std::cout << std::endl;
  }
}

aExp LConstraints::Constraint() const {
  return MakeANDFromExpressions(_Constraints);
}

void LConstraints::AddAddressConstraint(long long &currentAddress,
                                        const argo::Expression &e, int inc) {
  argo::Expression addr = ExpNum1(ItoS(currentAddress), fpointer_type);
  currentAddress += inc;
  _Constraints.push_back(argo::Expression::Equality(e, addr));
}

void LConstraints::AddConstraint(const argo::Expression &e, unsigned left,
                                 unsigned right) {
  if (CheckPointers) {
//    std::cout << "AddConstraint " << e.GetName() << " left= " << left
//              << " right= " << right << std::endl;
    argo::IntType iType = fint_type;
    argo::Expression leftside =
        argo::Expression::Equality(ExpLeft(e), ExpNum1(left, iType));
    argo::Expression rightside =
        argo::Expression::Equality(ExpRight(e), ExpNum1(right, iType));
    Add(leftside);
    Add(rightside);
  }
}

}
