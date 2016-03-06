//===-- LExpressionTransformation.h ----------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef FEXPRESSIONTRANSFORMATION_H
#define FEXPRESSIONTRANSFORMATION_H

#include <string>
#include <set>
#include <vector>

#include "lav/Internal/LTypes.h"




namespace lav
{
  aExp SetWidthZ                (caExp& e, unsigned width_old, unsigned width_new);
  aExp SetWidth                 (caExp& e, unsigned width_old, unsigned width_new);
  aExp ToInt                    (caExp& e, unsigned width);
  aExp ToPointer                (caExp& e, unsigned width);
  aExp PtrToInt                 (caExp& e);
  aExp IntToPtr                 (caExp& e);

  aExp MakeExpression           (caExp& e, const vaExp& operands, bool& found);
  aExp RenameExpressionVariables(caExp& e, int Context, const std::string& fname);
  aExp MakeANDFromExpressions   (const vaExp& exps); 

  aExp AddEqs                   (caExp& a, vaExp& eqs);
  void AddEqs                   (caExp& a, vaExp& eqs, aExp& result);

  aExp AddEqs                   (caExp& a, saExp& eqs);
  void AddEqs                   (caExp& a, saExp& eqs, aExp& result);
  bool IsGlobalVariable         (caExp& e);

  void CopySetIntoVector        (const saExp &set, vaExp &vector);
  void SetUnion                 (saExp &set1, const saExp &set2);

}//end of namespace

#endif
