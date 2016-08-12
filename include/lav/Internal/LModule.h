//===-- LModule.h ---------------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LMODULE_H
#define LMODULE_H

#include <vector>
#include <string>
#include <map>
#include <set>
#include "expression/expressions/Expression.h"
#include "lav/Internal/LConstraints.h"
#include "lav/Internal/LTypes.h"

namespace llvm {
class AliasAnalysis;
class Module;
class Function;
class DataLayout;
class GlobalVariable;
}

namespace lav {
class LFunction;
class InstructionInfoTable;

class LModule {
public:
  LModule(llvm::Module *m);
  ~LModule();

  void CalculateDescriptions();
  void CalculateConditions();

  llvm::Module *GetLLVMModule() const { return _Module; }
  const llvm::DataLayout *GetTargetData() const { return _TargetData; }
  const InstructionInfoTable *Infos() const { return _Infos; }
  LFunction *GetLFunction(llvm::Function *f) const;

  void PrintResults() const;
  std::ostream &Print(std::ostream &ostr) const;
  void Print(cStr &sFilename) const;
  void PrintHTML(cStr &name) const;
  std::ostream &PrintHTMLHeader(std::ostream &ostr) const;
  std::ostream &PrintHTMLFooter(std::ostream &ostr) const;
  void PrintHTMLHeader(cStr &sFilename) const;
  void PrintHTMLFooter(cStr &sFilename) const;

  const LConstraints &GlobalConstraints() const { return _GlobalConstraints; }
  LConstraints &GlobalConstraints() { return _GlobalConstraints; }

  const std::set<llvm::GlobalVariable *> &GlobalReferences() const {
    return _GlobalReferences;
  }
  std::set<llvm::GlobalVariable *> &GlobalReferences() {
    return _GlobalReferences;
  }
  const std::set<llvm::GlobalVariable *> &GlobalVariables() const {
    return _GlobalVariables;
  }
  std::set<llvm::GlobalVariable *> &GlobalVariables() {
    return _GlobalVariables;
  }

  argo::Expression GetGlobalConstraints() const;
  long long GetGlobalMemoryOffset() const { return _GlobalMemoryOffset; }
  long long &GlobalMemoryOffset() { return _GlobalMemoryOffset; }
  void GetAddresses(vaExp &eqs) const;

  void AddGlobalVariables();
  //FIXME
  std::map<std::string, unsigned> _Addresses;
  void Run();

private:
  void init();
  LModule(const LModule &m);
  LModule &operator=(const LModule &m);

private:
  llvm::Module *_Module;
  llvm::DataLayout *_TargetData;
  const InstructionInfoTable *_Infos;
  std::vector<LFunction *> _Functions;
  std::map<llvm::Function *, LFunction *> _LlvmToF;
  LConstraints _GlobalConstraints;
  std::set<llvm::GlobalVariable *> _GlobalReferences;
  std::set<llvm::GlobalVariable *> _GlobalVariables;
  long long _GlobalMemoryOffset;
};

} //end of namespace
#endif
