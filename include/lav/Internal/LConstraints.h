//===-- LConstraints.h ----------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LCONSTRAINTS_H
#define LCONSTRAINTS_H

#include "lav/Internal/LTypes.h"

#include <vector>
namespace lav {

class LConstraints {
public:
  inline void Add(caExp &e) { _Constraints.push_back(e); }
  inline void clear() { _Constraints.clear(); }
  aExp Constraint() const;
  void Print() const;
  void AddConstraint(caExp &e, unsigned left, unsigned right);
  void AddAddressConstraint(long long &currentAddress, caExp &e, int inc);

private:
  std::vector<aExp> _Constraints;

};

}

#endif
