//===-- FCommon.h ---------------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef FCOMMON_H
#define FCOMMON_H

#include "expression/expressions/Expression.h"
#include "lav/Internal/LTypes.h"
#include "llvm/Support/CommandLine.h"

#include "llvm/IR/Type.h"
namespace lav {

#define VALUE "v_"
#define LEFT "l_"
#define RIGHT "r_"

#define PROPOSITIONALV "PV_"
#define TRANSFORMATION "TR_"
#define ACTIVEBLOCK "AB_"
#define MEMORY "ME"
#define GLOBAL_MEMORY "G_M"
#define ADDRESS "AD_"
#define MALLOC "ML_"
#define VLA "VLA_"
#define CONTEXT "_CX_"
#define GLOBAL "GL"
#define ARR "AR"
#define VARNAME "fv_"

#define ExpZero aExp::Numeral("0")
#define ExpNumZeroInt aExp::Numeral("0", fint_type)
#define ExpNumZeroPtr aExp::Numeral("0", fpointer_type)
#define ExpNumZero(type) aExp::Numeral("0", type)
#define ExpFVar(s) aExp::FormulaVariable(s)

#define ExpNum(id) aExp::Numeral(id)
#define ExpNum1(id, width) aExp::Numeral(id, width)
#define ExpVar(name, width, relevant) aExp::Variable(name, width, relevant)
#define ExpArray(name)                                                         \
  aExp::array(ExpFVar(name), ExpNum(ItoS(GetBitWidth(fpointer_type))),         \
              ExpNum(ItoS(GetBitWidth(fint_type))))
#define AddressName(function_name, variable_name)                              \
  (ADDRESS + function_name + "_" + variable_name)
#define ArrayName(variable_name) (std::string(ARR) + "_" + variable_name)
#define GlobalName(variable_name) (std::string(GLOBAL) + "_" + variable_name)
#define ExpAddress(function_name, variable_name)                               \
  ExpVar(AddressName(function_name, variable_name), fpointer_type, false)
#define ExpGlobalAddress(variable_name)                                        \
  ExpAddress(std::string(GLOBAL), variable_name)
#define ExpString(str) aExp::String(str)
#define ExpPropVar ExpFVar(GetNextVariable())

#define ExpLeft(e) aExp::left(e, fint_type)
#define ExpRight(e) aExp::right(e, fint_type)
#define ExpGepi(e1, e2) aExp::add(e1, e2)

#define fpointer_type argo::sInt32Type
#define fint_type argo::sInt32Type
#define fchar_type argo::sInt8Type

} //end of namespace

namespace llvm {
class GlobalVariable;
class Value;
class Type;
class Instruction;
class Function;
}

class string;

namespace lav {

long long GetNext();
std::string ItoS(long long num);
std::string GetNextString();
std::string GetNextVariable();

std::string GetOperandName(llvm::Value *v);
argo::IntType GetIntType(const llvm::Type *t);
argo::IntType GetIntType(unsigned w);
argo::IntType GetIntType(const enum llvm::Type::TypeID t);
unsigned GetBitWidth(argo::IntType i);

aExp ActiveBlock(cStr &function, int id);
aExp Transformation(cStr &function, int id1, int id2);

std::string AddContext(cStr &name, int Context, cStr &f);
bool isRelevantGlobalVariable(llvm::GlobalVariable *gv);
bool isAddress(cStr &s);
bool isGlobalAddress(cStr &s);
bool isGlobalName(cStr &s);
bool isMalloc(cStr &s);
bool isArray(cStr &s);
bool isMalloc2(cStr &s);

std::string getMallocName(cStr &s);

llvm::Function *GetFunction(llvm::Instruction *instr, unsigned &numArgs);
aExp SimplifyExpression(caExp &e);
bool VariableIndex(caExp &e);
aExp MakeExp(cStr &name, cStr &function, cStr &s, int id, unsigned zero,
             const llvm::Type *t, int Context = 0, cStr &fname = "");
}

#endif
