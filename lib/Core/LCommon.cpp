//===-- FCommon.cpp
//--------------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <string>

#include "lav/Internal/LCommon.h"
#include "lav/Misc/misc.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/Support/CallSite.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

llvm::cl::opt<std::string> OutputFolder(
    "output-folder",
    llvm::cl::desc("LAV --- Name of the output folder (default = Output)"),
    llvm::cl::init("Output"));

llvm::cl::opt<std::string> StartFunction(
    "starting-function",
    llvm::cl::desc("LAV --- Name of the starting function (default = main)"),
    llvm::cl::init("main"));

llvm::cl::opt<std::string> InputFile(llvm::cl::desc("<input bytecode>"),
                                     llvm::cl::Positional, llvm::cl::init("-"));

llvm::cl::opt<bool>
    Model("model", llvm::cl::desc("LAV --- Generate model (default=false)"),
          llvm::cl::init(false));

llvm::cl::opt<bool>
    CheckPointers("check-pointers",
                  llvm::cl::desc("LAV --- Check pointers (default = true)"),
                  llvm::cl::init(true));

llvm::cl::opt<bool>
    TrackPointers("track-pointers",
                  llvm::cl::desc("LAV --- Track pointers (default = true)"),
                  llvm::cl::init(true));

llvm::cl::opt<bool>
    MemoryFlag("memory-flag",
               llvm::cl::desc("LAV --- Track memory (default = true)"),
               llvm::cl::init(true));

llvm::cl::opt<bool>
    CheckAssert("check-assert",
                llvm::cl::desc("LAV --- Check asserts (default = true)"),
                llvm::cl::init(true));

llvm::cl::opt<bool>
    FindFirstFlawed("find-first-flawed",
                    llvm::cl::desc("LAV --- Stop checking when first flawed "
                                   "command found (default = false)"),
                    llvm::cl::init(false));

llvm::cl::opt<bool>
    Students("students-mode",
             llvm::cl::desc("LAV --- Skip some checks (default = false)"),
             llvm::cl::init(false));

llvm::cl::opt<bool>
    PrintHtml("print-html",
              llvm::cl::desc("LAV --- Print HTML output (default = false)"),
              llvm::cl::init(false));
llvm::cl::opt<bool> CheckDivZero(
    "check-div-zero",
    llvm::cl::desc("LAV --- Check division-by-zero (default = true)"),
    llvm::cl::init(true));

llvm::cl::opt<bool> SkipInsideLoop(
    "skip-inside-loop",
    llvm::cl::desc(
        "LAV --- Skip checking inner loop unwindings (default = true)"),
    llvm::cl::init(true));

llvm::cl::opt<bool> CheckOverflow(
    "check-overflow",
    llvm::cl::desc("LAV --- Check for overflows (default = false)"),
    llvm::cl::init(false));

llvm::cl::opt<bool> CheckUnderflow(
    "check-underflow",
    llvm::cl::desc("LAV --- Check for underflows (default = false)"),
    llvm::cl::init(false));

// Autor: Branislava
// Dodato zbog paralelizacije

llvm::cl::opt<int> NumberThreads(
    "number-threads",
    llvm::cl::desc(
        "LAV --- Number of threads (default = hardware_concurrency)"),
    llvm::cl::init(0));
llvm::cl::opt<bool> EnableParallel(
    "enable-parallel",
    llvm::cl::desc("LAV --- Enable parallel solver calls (default = false)"),
    llvm::cl::init(false));



namespace lav {

void AbstractName(std::string &name) {
  for (unsigned i = 0; i < name.size(); i++)
    if ((name[i] >= 'a' && name[i] <= 'z') ||
        (name[i] >= 'A' && name[i] <= 'Z') ||
        (name[i] >= '0' && name[i] <= '9') || (name[i] == '_'))
      continue;
    else
      name[i] = '_';
}

thread_local std::map<llvm::Value *, std::string> _Names;

std::string GetOperandName(llvm::Value *v) {
  if (v->hasName()) {
    std::string name = v->getName();
    AbstractName(name);
    return name;
  }

  std::map<llvm::Value *, std::string>::iterator iter = _Names.find(v);
  if (iter != _Names.end())
    return iter->second;

  std::string s = GetNextVariable();
  _Names[v] = s;
  return s;
}

std::string GetOperandName2(llvm::Value *v) {
  if (v->hasName())
    return v->getName();

  std::map<llvm::Value *, std::string>::iterator iter = _Names.find(v);
  if (iter != _Names.end())
    return iter->second;

  std::string s = GetNextVariable();
  _Names[v] = s;
  return s;
}

//FIXME ovo nije dobra karakterizacija --- treba dozvoliti bilo koji konstantan
//izraz u kome ucestvuju
//numerali i adrese
bool VariableIndex(const argo::Expression &e) {

  if (e.IsNumeral())
    return false;

  for (unsigned i = 0; i < e.GetArity(); i++) {
    if (e[i].IsNumeral())
      continue;
    if (e[i].IsVariable() && isAddress(e[0].GetName()))
      continue;
    if (e[i].IsVariable() && !isAddress(e[0].GetName()))
      return true;
    if (!VariableIndex(e[i]))
      continue;
    else
      return true;
  }
  return false;
}

unsigned GetBitWidth(argo::IntType i) {
  return ((i < 3) ? 1 : (((i + 1) / 8) * 8));
}

argo::IntType GetIntType(unsigned w) {
  if (w == 1)
    return argo::sInt1Type;
  else if (w == 8)
    return argo::sInt8Type;
  else if (w == 16)
    return argo::sInt16Type;
  else if (w == 32)
    return argo::sInt32Type;
  else if (w == 64)
    return argo::sInt64Type;
  else {
    assert(0 && "XXX arbitrary bit widths unsupported");
    abort();
  }
  return argo::noInt;
}

argo::IntType GetIntType(const llvm::Type::TypeID t) {

  switch (t) {
  case llvm::Type::FloatTyID:
    return argo::sInt32Type;
  case llvm::Type::DoubleTyID:
    return argo::sInt64Type;
  case llvm::Type::X86_FP80TyID:
    return argo::sInt64Type; // XXX: needs to be fixed
  case llvm::Type::PointerTyID:
    return fpointer_type;
  default:
    return fpointer_type; //FIXME
  }
}

argo::IntType GetIntType(const llvm::Type *t) {
  if (t == NULL)
    return argo::noInt;

  if (t->getTypeID() == Type::IntegerTyID) {
    unsigned w = cast<llvm::IntegerType>(t)->getBitWidth();
    return GetIntType(w);
  }

  return GetIntType(t->getTypeID());
}

//FIXME
bool isRelevantGlobalVariable(llvm::GlobalVariable *gv) {
  std::string name = GetOperandName2(gv);
  if (name[0] == '.')
    return false;
  if ((name.size() > 5) && (name[0] == 'l') && (name[1] == 'l') &&
      (name[2] == 'v') && (name[3] == 'm') && (name[4] == '.'))
    return false;
  return true;
}

llvm::Function *GetFunction(llvm::Instruction *instr, unsigned &numArgs) {
  llvm::CallSite cs;
  if (instr->getOpcode() == llvm::Instruction::Call) {
    cs = llvm::CallSite(llvm::cast<llvm::CallInst>(instr));
  } else {
    cs = llvm::CallSite(llvm::cast<llvm::InvokeInst>(instr));
  }
  numArgs = cs.arg_size();
  return cs.getCalledFunction();
}

bool isAddress(const std::string &s) {
  std::string addr(ADDRESS);
  if ((s.size() <= addr.size()))
    return false;
  for (unsigned i = 0; i < addr.size(); i++)
    if (s[i] != addr[i])
      return false;
  return true;
}

bool isGlobalAddress(const std::string &s) {
  std::string addr(std::string(ADDRESS) + GLOBAL);
  if ((s.size() <= addr.size()))
    return false;
  for (unsigned i = 0; i < addr.size(); i++)
    if (s[i] != addr[i])
      return false;
  return true;
}

bool isGlobalName(const std::string &s) {
  std::string glob(GLOBAL);
  if ((s.size() <= glob.size()))
    return false;
  for (unsigned i = 0; i < glob.size(); i++)
    if (s[i] != glob[i])
      return false;
  return true;
}

std::string getMallocName(const std::string &s) {
  if (!isMalloc(s))
    return s;
  std::string name;
  std::string addr(MALLOC);
  unsigned i = addr.size();

  for (; i < s.size(); i++)
    name.push_back(s[i]);

  return name;
}
bool isMalloc(const std::string &s) {
  std::string addr(MALLOC);
  if ((s.size() <= addr.size()))
    return false;
  for (unsigned i = 0; i < addr.size(); i++)
    if (s[i] != addr[i])
      return false;
  return true;
}

bool isArray(const std::string &s) {
  std::string addr(ARR);
  if ((s.size() <= addr.size()))
    return false;
  for (unsigned i = 0; i < addr.size(); i++)
    if (s[i] != addr[i])
      return false;
  return true;
}

bool isMalloc2(const std::string &s) {
  std::string mall(MALLOC);
  const char *str1 = s.c_str();
  const char *str2 = mall.c_str();
  if (std::strstr(str1, str2) == NULL) {
    return false;
  }
  return true;
}

argo::Expression MakeExp(const std::string &name, const std::string &function,
                         const std::string &s, int id, unsigned zero,
                         const llvm::Type *t, int Context,
                         const std::string &fname) {
  std::string newname;
  if (zero == 0)
    newname = (name + function + "_" + s + "_" + ItoS(id) + "_0");
  else
    newname = (name + function + "_" + s + "_" + ItoS(id) + "_1");

  if (Context > 0)
    newname = AddContext(newname, Context, fname);

  if (s == MEMORY)
    return ExpArray(newname);
  else
    return ExpVar(newname, GetIntType(t), false);
}

long long GetNext() {
  thread_local static long long i = 0;
  return i++;
}

std::string GetNextString() { return ItoS(GetNext()); }

std::string GetNextVariable() { return VARNAME + GetNextString(); }

argo::Expression ActiveBlock(const std::string &function, int id) {
  return ExpFVar(ACTIVEBLOCK + function + std::string("_") + ItoS(id));
}

argo::Expression Transformation(const std::string &function, int id1, int id2) {
  return ExpFVar(TRANSFORMATION + function + std::string("_") + ItoS(id1) +
                 "_" + ItoS(id2));
}

std::string AddContext(const std::string &name, int Context,
                       const std::string &f) {
  if (Context > 0)
    return (name + "_" + f + CONTEXT + ItoS(Context));
  return name;
}

} //end of namespace
