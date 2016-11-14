//===-- InstructionInfoTable.h ----------------------------------*- C++ -*-===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LAV_LIB_INSTRUCTIONINFOTABLE_H
#define LAV_LIB_INSTRUCTIONINFOTABLE_H

#include <map>
#include <string>
#include <set>
#include "lav/Internal/LTypes.h"

namespace llvm {
class Function;
class Instruction;
class Module;
}

namespace lav {

/* Stores debug information for an Instruction */
class InstructionInfo {
public:
  unsigned id;
  cStr &file;
  unsigned line;
  unsigned assemblyLine;

public:
  InstructionInfo(unsigned _id, cStr &_file, unsigned _line,
                  unsigned _assemblyLine)
      : id(_id), file(_file), line(_line), assemblyLine(_assemblyLine) {}
};

class InstructionInfoTable {
  struct ltstr {
    bool operator()(cStr *a, cStr *b) const { return *a < *b; }
  };

  std::string dummyString;
  InstructionInfo dummyInfo;
  std::map<const llvm::Instruction *, InstructionInfo> infos;
  std::set<cStr *, ltstr> internedStrings;

private:
  cStr *internString(std::string s);
  bool getInstructionDebugInfo(const llvm::Instruction *I, cStr *&File,
                               unsigned &Line);

public:
  InstructionInfoTable(llvm::Module *m);
  ~InstructionInfoTable();

  unsigned getMaxID() const;
  const InstructionInfo &getInfo(const llvm::Instruction *) const;
  const InstructionInfo &getFunctionInfo(const llvm::Function *) const;
};

}

#endif
