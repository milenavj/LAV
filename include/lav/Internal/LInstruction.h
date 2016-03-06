//===-- LInstruction.h ----------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LINSTRUCTION_H
#define LINSTRUCTION_H

#include<vector>
#include "lav/Misc/misc.h"
#include "lav/Internal/LTypes.h"

namespace llvm
{ 
	class Instruction;
}


namespace lav
{

class InstructionInfo;
class LBlock;

class LInstruction
{


public:
	LInstruction(llvm::Instruction* i, const InstructionInfo* info, const LBlock* parent) : 
	          _Instruction(i), 
	          _Info(info), 
	          _Block(parent)
	{}
        
  void AddIntoStack(const vpInst& stack, const vUnsigned&);
  void AddIntoStack(LInstruction* s, unsigned context);

  inline       bool                StackEmpty      () const  { return (_Stack.size() == 0); }

  inline       vpInst&             Stack           ()        { return _Stack; }
  inline const vpInst&             Stack           () const  { return _Stack; }

  inline       vUnsigned&          Contexts        ()        { return _Contexts; }
  inline const vUnsigned&          Contexts        () const  { return _Contexts; }

  inline       llvm::Instruction*  Instruction     () const  { return _Instruction;}
  inline const InstructionInfo*    Info            () const  { return _Info;}
  inline const LBlock*             GetParentBlock  () const  { return _Block; }

               unsigned            AssemblyLine    () const ;
               std::string         GetModelFileName() const;
               vUnsigned           GetStackLines   () const;

  std::ostream&   Print     (std::ostream& ostr) const;
  std::ostream&   PrintHTML (std::ostream& ostr) const;
  void            PrintFlawedFoundResults(STATUS s) const;

private:
  void PrintFlawedFound       (std::ostream& ostr) const;
  void PrintFlawedFound       (cStr& fileName) const;
  void PrintFlawedFoundHTML   (std::ostream& f, STATUS s) const;
  void PrintFlawedFoundHTML   (cStr& fileName, STATUS s) const;

private:
  llvm::Instruction*                  _Instruction;
  vpInst                              _Stack;
  vUnsigned                           _Contexts;
  const InstructionInfo*	          _Info;
  const LBlock*                       _Block;
};

}//end of namespace
#endif
