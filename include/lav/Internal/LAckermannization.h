//===-- LAckermannization.h -----------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LACKERMANNIZATION_H
#define LACKERMANNIZATION_H

#include "lav/Internal/LExpressionTransformation.h"
#include "expression/expressions/Expression.h"

namespace argo { class Expression; }

namespace lav {

class LAckermannization {
public:

  static void GlobalAckermannization(caExp &a, caExp &b, aExp &result_a,
                                     aExp &result_b, saExp &ls, saExp &rs,
                                     std::map<aExp, aExp> &_Acks);
  static void GlobalAckermannization(caExp &cond, aExp &r_cond, saExp &ls,
                                     saExp &rs, std::map<aExp, aExp> &_Acks);
  static void Ackermannize(aExp &a, aExp &b, saExp &ls, saExp &rs);
  static void SimplifyLeftRight(caExp &e, saExp &eq);

private:
  static void Ackermannize(aExp &cond, saExp &ls, saExp &rs,
                           std::map<aExp, aExp> &_Acks);
  static void AckermannizeWithoutAbstraction(aExp &a, aExp &b,
                                             std::map<aExp, aExp> &_Acks,
                                             saExp &ls, saExp &rs);
  static aExp FlattenLR(caExp &a, saExp &lefts, saExp &rights,
                        std::map<aExp, aExp> &_Acks);
  static aExp SimplifyLeftRight(caExp &e, vaExp &eq, saExp &lefts,
                                saExp &rights, std::map<aExp, aExp> &_Acks);

};
} //end of namespace

#endif
