//===-- LLocalCondition.h -------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLOCALCONDITION_H
#define LLOCALCONDITION_H

#include <ostream>
#include "expression/expressions/Expression.h"
#include "lav/Misc/misc.h"
namespace lav {
class LInstruction;

class LLocalCondition {
public:
  LLocalCondition(caExp &l, caExp &r, const LInstruction *i, ERRKIND er,
                  STATUS s = UNCHECKED)
      : _LHS(l), _RHS(r), _Instruction(i), _ErrKind(er), _Status(s) {}

  inline aExp &LHS() { return _LHS; }
  inline caExp &LHS() const { return _LHS; }
  inline caExp &RHS() const { return _RHS; }
  inline const LInstruction *Instruction() const { return _Instruction; }
  inline STATUS Status() const { return _Status; }
  inline ERRKIND ErrorKind() const { return _ErrKind; }
  inline STATUS &Status() { return _Status; }

  std::ostream &Print(std::ostream &ostr) const;
  std::ostream &PrintHTML(std::ostream &f) const;

private:
  aExp _LHS;
  aExp _RHS;
  const LInstruction *_Instruction;
  ERRKIND _ErrKind;
  STATUS _Status;
};

} //end of namespace

#endif
