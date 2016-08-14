//===-- LFunction.h -------------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LFunction_H
#define LFunction_H

#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <set>

#include "expression/expressions/Expression.h"
#include "lav/Internal/LSolver.h"
#include "lav/Internal/LTypes.h"

namespace llvm {
class Function;
class BasicBlock;
class Instruction;
class AllocaInst;
class GlobalVariable;
}

namespace lav {
class LModule;
class LBlock;
class LVariableInfo;
class LConstraints;

class LFunction {
public:
  LFunction(llvm::Function *f, LModule *p);
  ~LFunction();

  void CalculateConditions();
  void CalculateDescriptions();
  void SetPostcondition();
  inline void SetNewContext() { ++_Context; }
  inline std::map<std::string, unsigned> &Addresses() { return _Addresses; }
  inline const std::vector<LBlock *> &GetBlocks() const { return _Blocks; }
  inline const llvm::Function *GetLLVMFunction() const { return _Function; }
  inline llvm::Function *GetLLVMFunction() { return _Function; }
  inline const LModule *GetParentModule() const { return _Parent; }
  inline LModule *GetParentModule() { return _Parent; }
  inline const std::vector<llvm::Instruction *> &GetReturnInstructions() const {
    return _ReturnInstructions;
  }
  inline int GetContext() const { return _Context; }
  inline const std::map<std::string, llvm::Type *> &GetReferences() const {
    return _References;
  }
  inline cStr &GetFunctionName() const { return _FunctionName; }
  inline const LConstraints &FunctionConstraints() const {
    return _FunctionConstraints;
  }
  inline LConstraints &FunctionConstraints() { return _FunctionConstraints; }
  inline aExp GetFunctionConstraints() const {
    return _FunctionConstraints.Constraint();
  }
  LBlock *GetLBlock(llvm::BasicBlock *bb) const;
  void GetAddresses(unsigned &current, std::vector<aExp> &eqs) const;
  std::ostream &Print(std::ostream &ostr) const;
  std::ofstream &PrintHTML(std::ofstream &f) const;
  std::ofstream &PrintHTMLHeader(std::ofstream &f) const;
  void PrintHTMLHeader(cStr &sFilename) const;

  void Run();
  void CalculateParallel();
  void AddFixedAddresses(LFunction *ff);

  aExp GetPostcondition();
  std::string GetNameOfNextVariable();
  unsigned GetLoopMax(unsigned loop);
  void AddLoopMax(const std::pair<unsigned, unsigned> &loop);

private:
  LFunction(const LFunction &f);
  LFunction &operator=(const LFunction &f);

  void init();
  void AddBlocks();
  void SetStoresInBlocks();
  void SetReturnInstructions();

  void AddLocalVariables(vStr &variables, vVarInfo &fvis);
  void AddGlobalVariables(vStr &variables, vVarInfo &fvis);
  void AddFunctionArguments(vStr &variables, vVarInfo &fvis);
  void SetReferences();
  void SetAddresses();
  bool AddAllocationFixedAddress(llvm::AllocaInst *ai, vStr &variables,
                                 vVarInfo &fvis);
  void AddReferenceConstraint(llvm::AllocaInst *ai);
  void AddGlobalReferenceConstraint(llvm::GlobalVariable *gv);
  void AddReferences(llvm::Instruction *instr, unsigned op);

  inline long long GetCurrentAddress() { return _MemCounter; }
  inline void SetNextAddress(int add) { _MemCounter += add; }
  void BlocksPostcondInSolverSetFalse();

private:
  llvm::Function *_Function;
  std::vector<LBlock *> _Blocks;
  std::map<llvm::BasicBlock *, LBlock *> _LlvmToF;
  LModule *_Parent;
  aExp _Postcondition;
  bool _PostconditionIsSet;
  bool _DescriptionsCalculated;
  bool _ConditionsCalculated;
  std::vector<llvm::Instruction *> _ReturnInstructions;
  int _Context;
  long long _VariableCounter;
  std::map<std::string, llvm::Type *> _References;
  std::map<std::string, unsigned> _Addresses;
  std::string _FunctionName;
  long long _MemCounter;
  LConstraints _FunctionConstraints;
  std::map<unsigned, unsigned> _LoopMax;
};

} //end of namespace

#endif
