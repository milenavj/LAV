//===-- LJump.h ----------------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LJUMP_H
#define LJUMP_H

#include <ostream>
#include <string>
#include <vector>
#include <set>

#include "expression/expressions/Expression.h"


namespace llvm
{ 
    struct BasicBlock;
}

namespace lav
{
class LInstruction;
class LBlock;

class LJump
{
public:
  LJump() : _Condition(argo::Expression::TOP()), _BB(NULL), _FB(NULL) {}
  LJump(llvm::BasicBlock* bbt) 
    : _Condition(argo::Expression::TOP()), 
      _BB(bbt), 
      _FB(NULL) 
      {}
  LJump( caExp& e, llvm::BasicBlock* bb) 
    : _Condition(e), 
      _BB(bb), 
      _FB(NULL) 
      {}
  LJump( caExp& e, llvm::BasicBlock* bb, LBlock* fb) 
    : _Condition(e), 
      _BB(bb), 
      _FB(fb) 
      {}

  inline caExp&                  Condition()               const { return _Condition; }
  inline llvm::BasicBlock*       BB()                      const { return _BB; }
  inline LBlock*                 FB()                      const { return _FB; }
  inline void                    SetFB(LBlock* fb)               { _FB = fb; }

  std::ostream&                  Print(std::ostream& ostr) const;
      
private:
  argo::Expression        _Condition;
  llvm::BasicBlock*       _BB;
  LBlock*                 _FB;
};

}//end of namespace

#endif
