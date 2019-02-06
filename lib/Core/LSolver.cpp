//===-- LSolver.cpp -------------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <iostream>
#include <fstream>
#include "llvm/Support/Timer.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/IR/Function.h"

#include "config.h"
#include "expression/expressions/Expression.h"
#include "expression/output/SMTFormater.h"
#include "solvers/solver-interfaces/expression.hpp"

#include "lav/Internal/LExpressionTransformation.h"
#include "lav/Internal/LCommon.h"
#include "lav/Internal/LSolver.h"
#include "lav/Internal/LAckermannization.h"
#include "lav/Internal/LBlock.h"
#include "lav/Internal/LInstruction.h"
#include "lav/Internal/LFunction.h"
#include "lav/Internal/InstructionInfoTable.h"

#if defined(YICES)
#include "solvers/solver-interfaces/yices/la-expression-yices.hpp"
#include "solvers/solver-interfaces/yices/bv-expression-yices.hpp"
#endif

#if defined(MATHSAT)
#include "solvers/solver-interfaces/mathsat/la-expression-mathsat.hpp"
#include "solvers/solver-interfaces/mathsat/bv-expression-mathsat.hpp"
#endif

#if defined(BOOLECTOR) || defined(BOOLECTOR_OLD)
#include "solvers/solver-interfaces/boolector/bv-expression-boolector.hpp"
#endif

#if defined(Z3)
#include "solvers/solver-interfaces/z3/bv-expression-z3.hpp"
#include "solvers/solver-interfaces/z3/la-expression-z3.hpp"
#endif

#include "solvers/syntax/symbol-table.hpp"

extern llvm::cl::opt<bool> Model;
extern llvm::cl::opt<bool> Students;
extern llvm::cl::opt<bool> TrackPointers;

namespace {

enum SolverType {
  BoolectorBV,
  BoolectorBVEUF,
  Z3BV,
  Z3BVACK,
  Z3LA,
  Z3LAACK,
  MSLA,
  MSLAACK,
  MSBV,
  MSBVACK,
  YicesLA,
  YicesLAACK,
  YicesBV,
  YicesBVACK,
  NoSolver
};

llvm::cl::opt<SolverType> solver(
    "solver", llvm::cl::desc("LAV --- Choose solver and theory"),
    llvm::cl::values(
#if defined(BOOLECTOR) || defined(BOOLECTOR_OLD)
        clEnumValN(BoolectorBV, "Boolector-BV-ARR-ACK",
                   "Boolector --- BitVectors & Arrays & Ackermanization"),
#endif
#if defined(BOOLECTOR)
        clEnumValN(
            BoolectorBVEUF, "Boolector-BV-ARR-EUF",
            "Boolector --- BitVectors & Arrays & Uninterpreted Functions"),
#endif
#ifdef Z3
        clEnumValN(Z3LA, "Z3-LA-ARR-EUF", "Z3        --- Linear Arithmetic  & "
                                          "Arrays & Uninterpreted Functions"),
        clEnumValN(
            Z3LAACK, "Z3-LA-ARR-ACK",
            "Z3        --- Linear Arithmetic  & Arrays & Ackermanization"),
        clEnumValN(
            Z3BV, "Z3-BV-ARR-EUF",
            "Z3        --- BitVectors & Arrays & Uninterpreted Functions"),
        clEnumValN(Z3BVACK, "Z3-BV-ARR-ACK",
                   "Z3        --- BitVectors & Arrays & Ackermanization"),
#endif
#ifdef MATHSAT
        clEnumValN(MSLA, "MS-LA-EUF",
                   "MathSAT   --- Linear Arithmetic & Uninterpreted Functions"),
        clEnumValN(MSLAACK, "MS-LA-ACK",
                   "MathSAT   --- Linear Arithmetic & Ackermanization"),
        clEnumValN(MSBV, "MS-BV-EUF",
                   "MathSAT   --- BitVectors & Uninterpreted Functions"),
        clEnumValN(MSBVACK, "MS-BV-ACK",
                   "MathSAT   --- BitVectors & Ackermanization"),
#endif
#ifdef YICES
        clEnumValN(YicesLA, "Yices-LA-EUF",
                   "Yices     --- Linear Arithmetic & Uninterpreted Functions"),
        clEnumValN(YicesLAACK, "Yices-LA-ACK",
                   "Yices     --- Linear Arithmetic & Ackermanization"),
        clEnumValN(YicesBV, "Yices-BV-EUF",
                   "Yices     --- BitVectors & Uninterpreted Functions"),
        clEnumValN(YicesBVACK, "Yices-BV-ACK",
                   "Yices     --- BitVectors & Ackermanization"),
#endif
#ifndef YICES
#ifndef Z3
#ifndef BOOLECTOR
#ifndef BOOLECTOR_OLD
#ifndef MATHSAT
        clEnumValN(NoSolver, "No-solver", "      No solver"),

#endif
#endif
#endif
#endif
#endif
        clEnumValEnd),
    llvm::cl::init(BoolectorBV));

llvm::cl::opt<bool> light("light",
                          llvm::cl::desc("LAV --- Check light (default=false)"),
                          llvm::cl::init(false));

llvm::cl::opt<bool>
    unreachable("track-unreachable",
                llvm::cl::desc("LAV --- Track unreachable (default=false)"),
                llvm::cl::init(false));

}

namespace lav {

thread_local static argo::SMTFormater SMTF;

//llvm::Timer IncrementalTime("Incremental Solving");
//llvm::Timer BlockIncrementalTime("Block Incremental Solving");
//llvm::Timer NonIncrementalTime("Non Incremental Solving");
//llvm::Timer AckermannizeTimer("Ackermannization");
//llvm::Timer GlobalAckermannizationTimer("Global Ackermannization");
//llvm::Timer NonIncrementalPreparationTime("NonIncrementalPreparation Time");
//llvm::Timer ResetTime("ResetTime");

bool LSolver::isZ3() {
  if ((solver == Z3LA) || (solver == Z3LAACK) || (solver == Z3BVACK) ||
      (solver == Z3BV))
    return true;
  else
    return false;
}
bool LSolver::isYices() {
  if ((solver == YicesLA) || (solver == YicesLAACK) || (solver == YicesBVACK) ||
      (solver == YicesBV))
    return true;
  else
    return false;
}

bool LSolver::isMathSAT() {
  if ((solver == MSLA) || (solver == MSLAACK) || (solver == MSBVACK) ||
      (solver == MSBV))
    return true;
  else
    return false;
}
bool LSolver::isBoolector() {
  if (solver == BoolectorBV || solver == BoolectorBVEUF)
    return true;
  else
    return false;
}

bool LSolver::isLATheory() {
  if ((solver == Z3LA) || (solver == Z3LAACK) || (solver == YicesLA) ||
      (solver == YicesLAACK) || (solver == MSLA) || (solver == MSLAACK))
    return true;
  else
    return false;
}

void LSolver::setFactory() { UrsaExp::setFactory(_Factory.get()); }

void LSolver::resetSolver() {
#if defined(BOOLECTOR) || defined(BOOLECTOR_OLD)
  if (isBoolector()) {
    UrsaMajor::BoolectorInstance::instance().reset();
  }
#endif

#if defined(Z3)
  if (isZ3())
    UrsaMajor::Z3Instance::instance().reset();
#endif

#if defined(YICES)
  if (isYices())
    UrsaMajor::YicesInstance::instance().reset();
#endif

#if defined(MATHSAT)
  if (isMathSAT())
    UrsaMajor::MathSATInstance::instance().reset();
#endif
}

void LSolver::reset() {
  //  ResetTime.startTimer();
  resetSolver();
  //  ResetTime.stopTimer();
  _Addresses.clear();
  _SymbolTable.clear();
  _Lefts.clear();
  _Rights.clear();
  _AbstractLA.clear();
}

void LSolver::SetAddresses(caExp &addr) {
  _Addresses.clear();
  if (addr.IsOR())
    return;
  if (addr.IsEquality())
    _Addresses.insert(std::pair<std::string, long>(
        addr[0].GetName(), addr[1].GetValue().get_si()));
  else
    for (unsigned i = 0; i < addr.GetArity(); i++) {
      if (addr[i].IsEquality())
        _Addresses.insert(std::pair<std::string, long>(
            addr[i][0].GetName(), addr[i][1].GetValue().get_si()));
      //ako nije = onda je or
    }

}

aExp LSolver::RenameExpressionVariables(caExp &e, int Context,
                                        cStr &FunctionName) {
  if (Context == 0)
    return e;
  if (e.IsTOP() || e.IsBOT() || e.IsNumeral())
    return e;

  if (e.IsVariable()) {
    if (IsGlobalVariable(e))
      return e;
    else
      return ExpVar(AddContext(e.GetName(), Context, FunctionName),
                    e.getIntType(), e.isRelevant());
  }
  if (e.IsFormulaVariable()) {
    if (IsGlobalVariable(e))
      return e;
    else
      return ExpFVar(AddContext(e.GetName(), Context, FunctionName));
  }

  if (e.GetArity() == 0)
    return e;

  //Apstrahovanje svih operanada
  vaExp operands;
  for (unsigned i = 0; i < e.GetArity(); i++)
    operands.push_back(RenameExpressionVariables(e[i], Context, FunctionName));
  bool b;
  return MakeExpression(e, operands, b);
}

std::string LSolver::AbstractName(caExp &e) {
  std::string name = e.GetName();
  for (unsigned i = 0; i < name.size(); i++)
    if ((name[i] >= 'a' && name[i] <= 'z') ||
        (name[i] >= 'A' && name[i] <= 'Z') ||
        (name[i] >= '0' && name[i] <= '9') || (name[i] == '_'))
      continue;
    else
      name[i] = '_';

  return name;
}

aExp LSolver::AbstractVariable(caExp &e) {
  assert(e.IsVariable());
  if (e.IsVariable()) {
    std::string name = AbstractName(e);
    return ExpVar(name, e.getIntType(), e.isRelevant());
  } else
    return e;
}

aExp LSolver::AbstractFormulaVariable(caExp &e) {
  assert(e.IsFormulaVariable());
  if (e.IsFormulaVariable()) {
    std::string name = AbstractName(e);
    return ExpFVar(name);
  } else
    return e;
}

int binarystring2signed(cStr &s) {
  int val = 0;
  std::string::const_iterator i = s.begin();
  for (; i != s.end(); i++)
    val = 2 * val + (*i != '0');
  return val;
}

//TODO prebacti negde drugde
bool isTransformation(cStr &s) { return StartsWith(s, TRANSFORMATION); }
bool isActive(cStr &s) { return StartsWith(s, ACTIVEBLOCK); }
std::string cut(cStr &s, cStr &begin) { return s.substr(begin.size()); }
std::string cutend(cStr &s, cStr &end) {

  if ((s.size() - end.size() - 1) > 0)
    //secemo i crticu
    return s.substr(0, s.size() - end.size() - 1);
  else
    return "";
}

void LSolver::GetTransform(stUrsaExp &symbolTable, vStr &transforamtions) {

  stUrsaExp::iterator i = symbolTable.begin(), ie = symbolTable.end();

  bool t = false;

  for (; i != ie; i++) {
    if (isTransformation(i->first)) {
      t = true;
      std::string s = i->second.getAssignment();
      if (s.size() == 1) {
        if (s[0] == '1') {
          std::string ss = cut(i->first, TRANSFORMATION);
          transforamtions.push_back(ss);
        }
      }
      continue;
    }
    if (t)
      break;
    /*   Ukoliko zelimo da izvucemo aktivne blokove, onda ovo active treba
         da bude treci argument funkcije
         if (isActive(i->first)) {
          std::string s = i->second.getAssignment();
          if (s.size() == 1)
            if (s[0] == '1') {
              std::string ss = cut(i->first, ACTIVEBLOCK);
              active.push_back(ss);
            }
        }*/
  }
}

//TODO prebaciti u drugu klasu
void WriteHint(std::ostream &ostr, ERRKIND ErrKind) {
  ostr << std::endl;
  ostr << "-------------------------" << std::endl;
  ostr << "HINT" << std::endl;
  ostr << "-------------------------" << std::endl;

  if (ErrKind == BUFFEROVERFLOW) {
    ostr << "A buffer overflow error occurs when" << std::endl;
    ostr << "trying to read or write outside the" << std::endl;
    ostr << "reserved memory for a buffer/array." << std::endl;
    ostr << "Check the boundaries for the array!" << std::endl;
  } else if (ErrKind == DIVISIONBYZERO) {
    ostr << "Possible division by zero!" << std::endl;
    ostr << "Check the value of the variable" << std::endl;
  } else if (ErrKind == ASSERT) {
    ostr << "Assertion failed!" << std::endl;
  } else if (ErrKind == NULLDEREFERENCING) {
    ostr << "Null pointer dereferencing error occurs when" << std::endl;
    ostr << "using an uninitialized pointer or" << std::endl;
    ostr << "when using a pointer for which memory is not allocated"
         << std::endl;
    ostr << "or when using a pointer for which memory is already freed"
         << std::endl;
    ostr << "Check the pointer before dereferencing it!" << std::endl;
  } else {
    ostr << "Strange error" << std::endl;
  }
  ostr << std::endl;

}

//TODO prebaciti u klasu LBlock
void WriteHeader(std::ostream &ostr, const LBlock *fb, ERRKIND erKind) {
  ostr << "function: " << fb->GetFunctionName() << std::endl
       << "error: " << sErrorKind(erKind) << std::endl;
  return;
}

void WriteLines(std::ostream &ostr, const LBlock *fb);

//TODO prebaciti u klasu LInstruction
void WriteLines(std::ostream &ostr, const LInstruction *fi) {
  if (fi == NULL)
    return;
  std::vector<unsigned> v = fi->GetStackLines();
  for (unsigned j = 0; j < v.size(); j++)
    ostr << v[j] << ": ";
}

//TODO prebaciti u klasu LBlock
void WriteLines(std::ostream &ostr, const LBlock *fb) {
  if (fb == NULL)
    return;
  std::vector<unsigned> v = fb->GetLastLineStack();
  for (unsigned j = 0; j < v.size(); j++)
    ostr << v[j] << ": ";
}

//TODO prebaciti u klasu LBlock
void LSolver::WriteBlockInfo(std::ostream &ostr, stUrsaExp &symbolTable,
                             const LBlock *fb, const LInstruction *fi,
                             std::string s) {
  if (fi == NULL) {
    WriteLines(ostr, fb);
  } else {
    WriteLines(ostr, fi);
  }

  WriteStoreValues(ostr, fb, symbolTable, s);
}

void LSolver::WriteTrace(std::ostream &f, std::vector<unsigned> &prvi,
                         std::vector<unsigned> &drugi, const LBlock *fb,
                         stUrsaExp &symbolTable) {
  if (fb == NULL)
    return;
  const LBlock *fb_tekuci = fb;
  unsigned blokID = fb_tekuci->Id();

  bool pronadjen = false;
  for (unsigned i = 0; i < drugi.size(); i++)
    if (drugi[i] == blokID)
      pronadjen = true;

  if (!pronadjen && (fb_tekuci->IsMerged())) {
    const LBlock *fb_limit = fb_tekuci;
    const LBlock *fb1 = fb_tekuci->MergedInto();
    fb_tekuci = fb1;
    blokID = fb_tekuci->Id();

    WriteMergedBlocks(f, fb_tekuci, symbolTable, fb_limit);
    //Ovde bi trebalo odstampati vrednosti na kraju bloka, problem je ako
    //su tu mergovani drugi blokovi i onda se stampa vrednost na kraju
    //svih tih blokova a ne na kraju ovog jednog bloka
    //krpljenje da se ne ispise nikakva vrednost, opet je bolje nego da se
    //ispise pogresna vrednost, treba nesto smisliti
    //  WriteBlockInfo(f, symbolTable, fb_tekuci);
    WriteLines(f, fb_tekuci);
    bool nadjen = false;
    for (unsigned i = 0; i < drugi.size(); i++)
      if (drugi[i] == blokID)
        nadjen = true;
    if (!nadjen) {
      const std::vector<LBlock *> &blocks =
          fb_tekuci->GetParentFunction()->GetBlocks();
      for (unsigned i = 0; i < blocks.size(); i++)
        if (blocks[i]->Id() == blokID) {
          fb_tekuci = blocks[i];
          nadjen = true;
          break;
        }
      if (!nadjen)
        return;
    }
    pronadjen = true;
  }

  if (!pronadjen && (fb_tekuci->HasMerged())) {
    blokID = fb_tekuci->Id();
    WriteMergedBlocks(f, fb_tekuci, symbolTable);
    WriteBlockInfo(f, symbolTable, fb_tekuci);
    bool nadjen = false;
    for (unsigned i = 0; i < drugi.size(); i++)
      if (drugi[i] == blokID)
        nadjen = true;
    if (!nadjen) {
      const std::vector<LBlock *> &blocks =
          fb_tekuci->GetParentFunction()->GetBlocks();
      for (unsigned i = 0; i < blocks.size(); i++)
        if (blocks[i]->Id() == blokID) {
          fb_tekuci = blocks[i];
          nadjen = true;
          break;
        }
      if (!nadjen)
        return;
    }
  }

  for (unsigned i = 0; i < drugi.size(); i++) {
    if (drugi[i] == blokID) {
      fb_tekuci = fb_tekuci->GetPredWithId(prvi[i]);
      if (fb_tekuci == NULL)
        break;
      blokID = fb_tekuci->Id();
      WriteMergedBlocks(f, fb_tekuci, symbolTable);
      WriteBlockInfo(f, symbolTable, fb_tekuci);
      i = -1;
    }
  }

}

std::string contextString(const LInstruction *fi, unsigned context) {
  if (fi->GetParentBlock()->GetContext() == 0)
    return "";
  return (std::string("_") + fi->GetParentBlock()->GetFunctionName() +
          std::string(CONTEXT) + ItoS(context));
}

//TODO prebaciti u klasu LBlock
void LSolver::WriteMergedBlocks(std::ostream &f, const LBlock *fb,
                                stUrsaExp &symbolTable,
                                const LBlock *fb_limit) {
  if (!(fb->HasMerged()) && fb->IsMerged()) {
    const LBlock *fb1 = fb->MergedInto();
    fb = fb1;
  }
  if (fb->HasMerged()) {
    const std::vector<LBlock *> &merged = fb->BlocksMerged();
    unsigned n = merged.size();
    bool nadjen = false;
    for (int i = (n > 0 ? n - 1 : -1); i >= 0; i--) {
      if (fb_limit) {
        if (fb_limit && merged[i] == fb_limit) {
          nadjen = true;
          continue;
        }
        if (!nadjen)
          continue;
        WriteBlockInfo(f, symbolTable, merged[i]);
      } else
        WriteBlockInfo(f, symbolTable, merged[i]);
    }
  }

}

void LSolver::GetModel(stUrsaExp &symbolTable, const LBlock *fb,
                       const LInstruction *fi, ERRKIND erKind) {
  if (!Model)
    return;

  std::string filename = fi->GetModelFileName();
  std::ofstream f(filename.c_str());

  WriteHeader(f, fb, erKind);
  WriteBlockInfo(f, symbolTable, fb, fi);

  vStr transformations;
  GetTransform(symbolTable, transformations);

  vStr transforms;
  std::string fname = fb->GetFunctionName();
  GetFunctionTransforms(transformations, fname, transforms);

  std::vector<unsigned> prvi, drugi;
  GetEdges(transforms, prvi, drugi);

  WriteTrace(f, prvi, drugi, fb, symbolTable);

  //da li je greska u pozivu funkcije
  if (fi->Stack().size() > 0) {

    //jeste, greska je u pozivu funkcije
    const std::vector<LInstruction *> &stack = fi->Stack();
    const std::vector<unsigned> &con = fi->Contexts();
    const LInstruction *fprint = fi;
    vStr contexts;
    vStr fnames;
    std::vector<const LBlock *> LBlocks;
    std::vector<const LInstruction *> LInstructions;

    fprint = fi;
    //pronadji sve relevantne kontekste
    for (unsigned k = 0; k < stack.size(); k++) {
      vStr vs;
      //ovaj kontekst ovde ne valja, nije jasan zasto bi bio poslednji, mozda bi
      //trebalo da ima svoj
      std::string s = contextString(fprint, con[con.size() - 1]);
      for (unsigned j = k; j < stack.size(); j++)
        s += contextString(stack[j], con[j]);

      if (s.size() == 0)
        continue;
      contexts.push_back(s);
      fnames.push_back(fprint->GetParentBlock()->GetFunctionName());
      LBlocks.push_back(fprint->GetParentBlock());
      LInstructions.push_back(fprint);
      fprint = stack[k];
    }

    for (int i = contexts.size() - 1; i >= 0; i--) {
      vStr transforms;
      std::string fname = fnames[i];
      GetFunctionTransforms(transformations, fnames[i], transforms,
                            contexts[i]);

      std::vector<unsigned> prvi, drugi;
      GetEdges(transforms, prvi, drugi);

      const std::vector<unsigned> &v = stack[i]->GetStackLines();
      f << "Function " << fname << " is called at line no. " << v[0];
      f << "\nIn function " << fname;
      f << " the trace is \n";

      WriteBlockInfo(f, symbolTable, LBlocks[i], LInstructions[i], contexts[i]);
      WriteTrace(f, prvi, drugi, LBlocks[i], symbolTable);

      f << std::endl;
    }
  } //end if
  if (Students)
    WriteHint(f, erKind);
  f.close();
}

UrsaExp LSolver::ExportVariable(caExp &e, stUrsaExp &symbolTable, cStr &s) {

      if (isAddress(e.GetName())) {
    if (s == "") {
      if (_Addresses.find(e.GetName()) != _Addresses.end()) {

        unsigned addr = _Addresses[e.GetName()];
        return UrsaExp::integerGround(addr, fpointer_type);
      }
    }
  }
  std::string name = s + e.GetName();

  if (symbolTable.contains(name)) {
    return symbolTable.get(name);
  } else {
    UrsaExp ue;
    if (e.getIntWidth() > 1) {
        if (e.isSigned()) {
        //fixme srediti tipove
        (TrackPointers ? ue = UrsaExp::signedSymbolic(name, e.getIntWidth())
: ue = UrsaExp::signedSymbolic(name, fint_type));
        }
      else
        (TrackPointers ? ue = UrsaExp::unsignedSymbolic(name, e.getIntWidth())
: ue = UrsaExp::unsignedSymbolic(name, fint_type));
}
    else
      ue = UrsaExp::booleanSymbolic(name);
    symbolTable.set(name, ue);
    return ue;
  }

}

void PrintA(caExp &a, cStr &s) {
  std::cout << s << " --------------eeABSTRACTED--------------" << std::endl;
  a.Print(&SMTF, std::cout);
  std::cout << std::endl;
  std::cout << "--------------eeABSTRACTED--------------" << std::endl;

}

void PrintAB(caExp &a, caExp &b) {
  PrintA(a, "aaaaa");
  PrintA(b, "bbbbb");
}

UrsaExp LSolver::ExportExpressionBV(caExp &e, stUrsaExp &symbolTable, ERRKIND erKind) {

//    std::cout << "ExportExpressionBV" << std::endl;
//    PrintA(e, "e" );
//    std::cout  << std::endl;

    if (e.IsTOP())
    return UrsaExp::booleanGround(true);

  if (e.IsBOT())
    return UrsaExp::booleanGround(false);

  if (e.IsNumeral()) //FIXME ovo je bezveze
      {
      if (e.getIntWidth() < 3) {
      if (e.GetValue().get_si() == 0)
        return UrsaExp::booleanGround(false);
      else
        return UrsaExp::booleanGround(true);
    }

    if (e.isSigned())
      return
          //fixme ovo izbaciti i srediti tipove
          (TrackPointers
               ? UrsaExp::integerGround((signed long) e.GetValue().get_si(),
                                        e.getIntWidth())
               : UrsaExp::integerGround((signed long) e.GetValue().get_si(),
                                        fint_type));
    else
      return
          //fixme ovo izbaciti i srediti tipove
          (TrackPointers
               ? UrsaExp::integerGround((unsigned long) e.GetValue().get_si(),
                                        e.getIntWidth())
               : UrsaExp::integerGround((unsigned long) e.GetValue().get_si(),
                                        fint_type));
  }

  if (e.IsVariable()) {
    return ExportVariable(e, symbolTable);
  }
  if (e.IsFormulaVariable()) {

    if (symbolTable.contains(e.GetName()))
      return symbolTable.get(e.GetName());
    else {
      UrsaExp ue = UrsaExp::booleanSymbolic(e.GetName());
      symbolTable.set(e.GetName(), ue);
      return ue;
    }
  }

  if (e.isArray()) {
    if (HasArrays()) {
      std::string arrayName = ArrayName(e[0].GetName());
      if (symbolTable.contains(arrayName))
        return symbolTable.get(arrayName);
      else {
        //FIXME procitati domain i range od niza i to postaviti ovde
        UrsaExp array = UrsaExp::array(arrayName, GetBitWidth(fpointer_type),
                                       GetBitWidth(fint_type));
        symbolTable.set(arrayName, array);
        return array;
      }
    } else
      return GetAbstractASS(e);

  }

  std::vector<UrsaExp> operands;
  for (unsigned i = 0; i < e.GetArity(); i++) {
    operands.push_back(ExportExpressionBV(e[i], symbolTable, erKind));
  }

  if (e.isSelect()) {
    if (HasArrays())
      return UrsaExp::select(operands[0], operands[1]);
    else
      return GetAbstractASS(e);
  }

  if (e.isStore()) {
    if (HasArrays())
      return UrsaExp::store(operands[0], operands[1], operands[2]);
    else
      return GetAbstractASS(e);
  }

  if (e.isGepi())
    return operands[0] + operands[1];
  if (e.isAdd())
    return operands[0] + operands[1];
  if (e.isSub())
    return operands[0] - operands[1];
  if (e.isMul())
    return operands[0] * operands[1];
  if (e.isSdiv())
    return operands[0].sdiv(operands[1]);
  if (e.isUdiv())
    return operands[0].udiv(operands[1]);
  if (e.isSrem())
    return operands[0].srem(operands[1]);
  if (e.isUrem())
    return operands[0].urem(operands[1]);
  if (e.isUlt())
    return operands[0].ult(operands[1]);
  if (e.isUle())
    return operands[0].ule(operands[1]);
  if (e.isUgt())
    return operands[0].ugt(operands[1]);
  if (e.isUge())
    return operands[0].uge(operands[1]);
  if (e.isSlt())
    return operands[0].slt(operands[1]);
  if (e.isSle())
    return operands[0].sle(operands[1]);
  if (e.isSgt())
    return operands[0].sgt(operands[1]);
  if (e.isSge())
    return operands[0].sge(operands[1]);
  if (e.isBitAnd())
    return operands[0] & operands[1];
  if (e.isBitNot())
  {
      //ovo je zbog Z3
      if(e[0].IsBOT()) return UrsaExp::booleanGround(true);
      return ~operands[0];
  }
  if (e.isBitOr())
    return operands[0] | operands[1];
  if (e.isBitXor())
    return operands[0] ^ operands[1];
  if (e.isShiftL())
    return operands[0] << operands[1];
  if (e.islShiftR())
    return operands[0].lShiftR(operands[1]);
  if (e.isaShiftR())
    return operands[0].aShiftR(operands[1]);

  if (e.IsITE()) {
    if (e[0].IsTOP())
      return operands[1];
    else if (e[0].IsBOT())
      return operands[2];
    else {
      //fixme tip
      unsigned width = GetBitWidth(fint_type);
      if (e[1].IsVariable() || e[1].IsNumeral())
        width = GetBitWidth(e[1].getIntType());
      else if (e[2].IsVariable() || e[2].IsNumeral())
        width = GetBitWidth(e[2].getIntType());
      return operands[0].ite(operands[1], operands[2], width);
    }
  }

  if (e.isSext())
    return operands[0].sext(e[1].GetValue().get_si());
  if (e.isZext()) {
/*
  //ovde nesto nije u redu sa z3-om jer ne dozvoljava
  //zext na bool-u sto je dozvoljeno u c-u pa takvi
  //izrazi nastaju, boolector za ovo ne pravi problem
  //na primer, ovo je za z3 neispravno jer se zext primenjuje
  //na a<=b koje je tipa bool, mozda je novi z3 ovo ispravio pa
  //nema ga smisla budziti za ovaj specijalni slucaj
  //  i = (a <= b);
  //  assert(i!=1);
  if (isZ3()) {
    //ako je bool ne smem zext(bool) vec mora ite(bool ? 1,0)
    if ((e[0].IsNumeral() ||
         e[0].IsVariable())) { //&& e[0].getIntWidth() < 3) {
      int width = e[1].GetValue().get_si();
      UrsaExp eu1 = UrsaExp::integerGround(1, width);
      UrsaExp eu0 = UrsaExp::integerGround(0, width);
      return operands[0].ite(eu1, eu0, width);
    } else if (e[0].IsITE()) {
      //zext (ITE bot  0  1 ) 32 )
      if (e[0][0].IsBOT() && e[0][2].IsNumeral()) {
        int width = e[1].GetValue().get_si();
        UrsaExp eu0 =
            UrsaExp::integerGround(e[0][2].GetValue().get_si(), width);
        return eu0;
      } else if (e[0][0].IsTOP() && e[0][1].IsNumeral()) {
        int width = e[1].GetValue().get_si();
        UrsaExp eu0 =
            UrsaExp::integerGround(e[0][1].GetValue().get_si(), width);
        return eu0;
      } else {
        int width = e[1].GetValue().get_si();
*/
      if (isZ3()) {
        //ako je bool ne smem zext(bool) vec mora ite(bool ? 1,0)
        if ((e[0].IsNumeral() || e[0].IsVariable()) && e[0].getIntWidth() < 3) {
            int width = e[1].GetValue().get_si();
          UrsaExp eu1 = UrsaExp::integerGround(1, width);
          UrsaExp eu0 = UrsaExp::integerGround(0, width);
          return operands[0].ite(eu1, eu0, width);
        }
      }
      return operands[0].zext(e[1].GetValue().get_si());
  }
  if (e.isExtract())
    return operands[0]
        .extract(e[1].GetValue().get_si(), e[2].GetValue().get_si());

  if (e.IsNOT()) {
      if(e[0].isAdd() || e[0].isMul() || e[0].isSdiv() || e[0].isUdiv() || e[0].isSub())
      {
          UrsaExp exported_exp;
          errkind err = (e[0].isAdd() ? OVERFLOW_ADD : (e[0].isMul() ? OVERFLOW_MUL : (e[0].isSub() ? OVERFLOW_SUB : (e[0].isUdiv() ? OVERFLOW_UDIV : OVERFLOW_SDIV))) ) ;
          ExportOU(e[0], exported_exp, err, symbolTable);
          return !exported_exp;
              //#####
      }
    return !operands[0];
  }
  if (e.IsIMPL())
    return (!operands[0] || operands[1]);
  if (e.IsIFF())
    return ((!operands[0] || operands[1]) && (operands[0] || !operands[1]));
  if (e.IsXOR())
    return !((!operands[0] || operands[1]) && (operands[0] || !operands[1]));
  if (e.IsEquality())
    return (operands[0] == operands[1]);
  if (e.IsDisequality())
    return (operands[0] != operands[1]);

  if (e.IsAND()) {
    UrsaExp eu = operands[0];
    for (unsigned i = 1; i < operands.size(); i++)
      eu = (eu && operands[i]);
    return eu;
  }

  if (e.IsOR()) {
    UrsaExp eu = operands[0];
    for (unsigned i = 1; i < operands.size(); i++)
      eu = (eu || operands[i]);
    return eu;
  }

  if (e.IsFunction() && (e.isLeft() || e.isRight())) {
    std::vector<UrsaMajor::TypedId> args;
    for (unsigned i = 0; i < operands.size(); i++) {
      //FIXME
      // args.push_back(UrsaMajor::TypedId(UrsaMajor::Type(BITVECTOR,
      // e[i].getIntWidth()), ItoS(i)));
      args.push_back(
          UrsaMajor::TypedId(UrsaMajor::Type(BITVECTOR, fint_type), ItoS(i)));
    }

    UrsaMajor::Function f(UrsaMajor::Type(BITVECTOR, e.getIntWidth()),
                          e.GetName(), args, 0);
    UrsaExp uf = UrsaExp::uninterpretedFunction(f, operands);
    return uf;
  }

  if (e.IsFunction() && (solver != BoolectorBV) &&
      (e.GetName() == "strlen" || e.GetName() == "toupper" ||
       e.GetName() == "tolower" || e.GetName() == "atoi" ||
       e.GetName() == "round" || e.GetName() == "trunc" ||
       e.GetName() == "ceil" || e.GetName() == "floor" ||
       e.GetName() == "sqrt" || e.GetName() == "sin" || e.GetName() == "cos")) {
    std::vector<UrsaMajor::TypedId> args;
    //fixme ovde mozda treba e[0].getIntWidth
    args.push_back(UrsaMajor::TypedId(
        UrsaMajor::Type(BITVECTOR, e.getIntWidth()), ItoS(0)));
    //fixme ovde mozda treba e[0].getIntWidth
    UrsaMajor::Function f(UrsaMajor::Type(BITVECTOR, e.getIntWidth()),
                          e.GetName(), args, 0);
    UrsaExp uf = UrsaExp::uninterpretedFunction(f, operands);
    return uf;

  }

  if (e.IsFunction() && (solver != BoolectorBV) && (e.GetName() == "pow")) {

    std::vector<UrsaMajor::TypedId> args;
    //fixme ovde mozda treba e[0].getIntWidth
    args.push_back(UrsaMajor::TypedId(
        UrsaMajor::Type(BITVECTOR, e.getIntWidth()), ItoS(0)));
    args.push_back(UrsaMajor::TypedId(
        UrsaMajor::Type(BITVECTOR, e.getIntWidth()), ItoS(1)));
    //fixme ovde mozda treba e[0].getIntWidth
    UrsaMajor::Function f(UrsaMajor::Type(BITVECTOR, e.getIntWidth()),
                          e.GetName(), args, 0);
    UrsaExp uf = UrsaExp::uninterpretedFunction(f, operands);
    return uf;

  }

  if (e.IsFunction() && (solver != BoolectorBV) &&
      (e.GetName().find("__UF__", 0)) != (std::string::npos)) {
    std::size_t found = e.GetName().find_first_of("8765432");
    if (found != std::string::npos) {
      std::vector<UrsaMajor::TypedId> args;
      int n = e.GetName()[found] - '0';
      for (int i = 0; i < n; i++)
        args.push_back(UrsaMajor::TypedId(
            UrsaMajor::Type(BITVECTOR, e.getIntWidth()), ItoS(i)));
      UrsaMajor::Function f(UrsaMajor::Type(BITVECTOR, e.getIntWidth()),
                            e.GetName(), args, 0);
      UrsaExp uf = UrsaExp::uninterpretedFunction(f, operands);
      return uf;
    }

  }

  if (e.IsFunction() && (solver != BoolectorBV)) {
    std::vector<UrsaMajor::TypedId> args;
    for (unsigned i = 0; i < operands.size(); i++) {
      //prilikom eksporta funkcija ovde moze da bude problem sa tipovima!!!
      args.push_back(UrsaMajor::TypedId(
          UrsaMajor::Type(BITVECTOR, e.getIntWidth()), ItoS(i)));
    }

    UrsaMajor::Function f(UrsaMajor::Type(BITVECTOR, e.getIntWidth()),
                          e.GetName(), args, 0);
    UrsaExp uf = UrsaExp::uninterpretedFunction(f, operands);
    return uf;
  } else {
    //fixme ovo ovako ne treba da bude ovde, treba da se prebaci u kreiranje
    //formula a ne da stoji u eksportu
    std::string name = e.GetName();
    for (unsigned i = 0; i < e.GetArity(); i++)
      name += e[i].GetName();

    return ExportVariable(
        AbstractVariable(ExpVar(name, GetIntType(e.getIntWidth()), false)),
        symbolTable);
  }

  //ovde ne bi trebalo da moze da se dodje
  assert(0 && "ExportExpressionBV");
  return UrsaExp::booleanGround(true);

}

UrsaExp LSolver::GetAbstractASS(caExp &e) {
  if (_AbstractLA.find(e) == _AbstractLA.end()) {
    UrsaExp um = UrsaExp::signedSymbolic(GetNextVariable(), e.getIntWidth());
    _AbstractLA.insert(std::pair<aExp, UrsaExp>(e, um));
    return um;
  } else
    return _AbstractLA[e];

}

UrsaExp LSolver::GetAbstractLA(caExp &e) {
  if (_AbstractLA.find(e) == _AbstractLA.end()) {
    UrsaExp um = UrsaExp::signedSymbolic(GetNextVariable(), e.getIntWidth());
    _AbstractLA.insert(std::pair<aExp, UrsaExp>(e, um));
    return um;
  } else
    return _AbstractLA[e];
}

UrsaExp LSolver::ExportExpressionLA(caExp &e, stUrsaExp &symbolTable) {

  if (e.IsTOP())
    return UrsaExp::booleanGround(true);

  if (e.IsBOT())
    return UrsaExp::booleanGround(false);

  if (e.IsNumeral()) { //FIXME ovo je bezveze
    if (e.getIntWidth() == 1 && e.GetValue().get_si() == 0)
      return UrsaExp::booleanGround(false);
    if (e.getIntWidth() == 1 && e.GetValue().get_si() != 0)
      return UrsaExp::booleanGround(true);

    if (e.isSigned())
      return UrsaExp::integerGround((signed long) e.GetValue().get_si(),
                                    e.getIntWidth());
    else
      return UrsaExp::integerGround((unsigned long) e.GetValue().get_si(),
                                    e.getIntWidth());
  }

  if (e.IsVariable()) {
    return ExportVariable(e, symbolTable);
  }
  if (e.IsFormulaVariable()) {
    if (symbolTable.contains(e.GetName()))
      return symbolTable.get(e.GetName());
    else {
      UrsaExp ue = UrsaExp::booleanSymbolic(e.GetName());
      symbolTable.set(e.GetName(), ue);
      return ue;
    }
  }

  if (e.isArray()) {
    if (HasArrays()) {
      if (symbolTable.contains(e[0].GetName()))
        return symbolTable.get(e[0].GetName());
      else {
        //FIXME procitati domain i range od niza i to postaviti ovde
        UrsaExp array = UrsaExp::array(
            e[0].GetName(), GetBitWidth(fpointer_type), GetBitWidth(fint_type));
        symbolTable.set(e[0].GetName(), array);
        return array;
      }
    } else
      return GetAbstractASS(e);
  }

  std::vector<UrsaExp> operands;
  for (unsigned i = 0; i < e.GetArity(); i++)
    operands.push_back(ExportExpressionLA(e[i], symbolTable));

  if (e.isSelect()) {
    if (HasArrays())
      return UrsaExp::select(operands[0], operands[1]);
    else
      return GetAbstractASS(e);
  }

  if (e.isStore()) {
    if (HasArrays())
      return UrsaExp::store(operands[0], operands[1], operands[2]);
    else
      return GetAbstractASS(e);
  }

  if (e.isAdd() || e.isGepi())
    return operands[0] + operands[1];
  if (e.isSub())
    return operands[0] - operands[1];
  if (e.isMul()) {
    if (e[0].IsNumeral() || e[1].IsNumeral()) {
      unsigned long m1;
      UrsaExp um1;
      if (e[0].IsNumeral()) {
        um1 = operands[1];
        m1 = (unsigned long) e[0].GetValue().get_si();
      } else {
        um1 = operands[0];
        m1 = (unsigned long) e[1].GetValue().get_si();
      }
      UrsaExp result = um1;
      for (unsigned int i = 1; i < m1; i++)
        result = result + um1;
      return result;
      //return operands[0]*operands[1];
    }
  }

  if (e.isUlt())
    return operands[0].ult(operands[1]);
  if (e.isUle())
    return operands[0].ule(operands[1]);
  if (e.isUgt())
    return operands[0].ugt(operands[1]);
  if (e.isUge())
    return operands[0].uge(operands[1]);
  if (e.isSlt())
    return operands[0].slt(operands[1]);
  if (e.isSle())
    return operands[0].sle(operands[1]);
  if (e.isSgt())
    return operands[0].sgt(operands[1]);
  if (e.isSge())
    return operands[0].sge(operands[1]);

  if (e.IsNOT() || e.isBitNot())
    return !operands[0];
  if (e.IsIMPL())
    return (!operands[0] || operands[1]);
  if (e.IsIFF())
    return ((!operands[0] || operands[1]) && (operands[0] || !operands[1]));
  if (e.IsXOR())
    return !((!operands[0] || operands[1]) && (operands[0] || !operands[1]));
  if (e.IsEquality())
    return operands[0] == operands[1];
  if (e.IsDisequality())
    return (operands[0] != operands[1]);

  if (e.IsAND()) {
    UrsaExp eu = operands[0];
    for (unsigned i = 1; i < operands.size(); i++)
      eu = (eu && operands[i]);
    return eu;
  }

  if (e.IsOR()) {
    UrsaExp eu = operands[0];
    for (unsigned i = 1; i < operands.size(); i++)
      eu = (eu || operands[i]);
    return eu;
  }

  if (solver == Z3LAACK || solver == YicesLAACK || solver == MSLAACK) {
    if (e.isMul() || e.isSdiv() || e.isUdiv() || e.isSrem() || e.isUrem() ||
        e.isBitAnd() || e.isBitOr() || e.isBitXor() || e.isShiftL() ||
        e.islShiftR() || e.isaShiftR())
      return GetAbstractLA(e);
  }

  if (e.isExtract()) {
    if (e[0].IsNumeral())
      return operands[0]
          .extract(e[1].GetValue().get_si(), e[2].GetValue().get_si());
    else
      return operands[0];
  }

  if (e.IsITE()) {
    //ovde sirina nije ni bitna
    unsigned width = GetBitWidth(fint_type);
    if (e[1].IsVariable() || e[1].IsNumeral())
      width = GetBitWidth(e[1].getIntType());
    else if (e[2].IsVariable() || e[2].IsNumeral())
      width = GetBitWidth(e[2].getIntType());
    return operands[0].ite(operands[1], operands[2], width);
  }

  if (e.isSext())
    return operands[0];

  if (e.isZext()) {
    if (isZ3()) {
      //ako je bool ne smem zext(bool) vec mora ite(bool ? 1,0)
      if ((e[0].IsNumeral() || e[0].IsVariable()) && e[0].getIntWidth() < 3) {
          int width = e[1].GetValue().get_si();
        UrsaExp eu1 = UrsaExp::integerGround(1, width);
        UrsaExp eu0 = UrsaExp::integerGround(0, width);
        return operands[0].ite(eu1, eu0, width);
      }
    }
    return operands[0]; //ovo moze da pravi problem kod z3
  }

  if (e.IsFunction()) {
    std::vector<UrsaMajor::TypedId> args;
    for (unsigned i = 0; i < e.GetArity(); i++) {
      args.push_back(UrsaMajor::TypedId(
          UrsaMajor::Type(UNSIGNED, e[i].getIntWidth()), ItoS(i)));
    }

    UrsaMajor::Function f(UrsaMajor::Type(UNSIGNED, e.getIntWidth()),
                          e.GetName(), args, 0);
    UrsaExp uf = UrsaExp::uninterpretedFunction(f, operands);
    return uf;
  }
  //ovde ne bi trebalo da moze da se dodje
  assert(0 && "ExportExpressionLA!");
  return UrsaExp::booleanGround(true);

}

STATUS LSolver::GetStatus(SATISFIABILITY sat1, SATISFIABILITY sat2) {
  if ((sat1 == UNKNOWN) || (sat2 == UNKNOWN))
    return ERROR;
  if ((sat1 == UNSAT) && (sat2 == SAT))
    return SAFE;
  if ((sat1 == SAT) && (sat2 == UNSAT))
    return FLAWED;
  if ((sat1 == SAT) && (sat2 == SAT))
    return UNSAFE;
  if ((sat1 == UNSAT) && (sat2 == UNSAT))
    return UNREACHABLE;
  return ERROR;
}

LSolver::LSolver() {
  switch (solver) {
#if defined(BOOLECTOR) || defined(BOOLECTOR_OLD)
  case BoolectorBV:
  case BoolectorBVEUF:
    _Factory =
        LSolver::ExpFactory(new UrsaMajor::BVExpressionFactoryBoolector());
    _BV = true;
    break;
#endif

#if defined(Z3)
  case Z3BV:
  case Z3BVACK:
    _Factory = LSolver::ExpFactory(new UrsaMajor::BVExpressionFactoryZ3());
    _BV = true;
    break;
  case Z3LA:
  case Z3LAACK:
    _Factory = LSolver::ExpFactory(new UrsaMajor::LAExpressionFactoryZ3());
    _BV = false;
    break;
#endif

#if defined(YICES)
  case YicesLA:
  case YicesLAACK:
    _Factory = LSolver::ExpFactory(new UrsaMajor::LAExpressionFactoryYices());
    _BV = false;
    break;
  case YicesBV:
  case YicesBVACK:
    _Factory = LSolver::ExpFactory(new UrsaMajor::BVExpressionFactoryYices());
    _BV = true;
    break;
#endif

#if defined(MATHSAT)
  case MSLA:
  case MSLAACK:
    _Factory = LSolver::ExpFactory(new UrsaMajor::LAExpressionFactoryMathSAT());
    _BV = false;
    break;
  case MSBV:
  case MSBVACK:
    _Factory = LSolver::ExpFactory(new UrsaMajor::BVExpressionFactoryMathSAT());
    _BV = true;
    break;
#endif

  default:
    ;
#if defined(BOOLECTOR) || defined(BOOLECTOR_OLD)
    _Factory =
        LSolver::ExpFactory(new UrsaMajor::BVExpressionFactoryBoolector());
    _BV = true;
    break;
#endif
#if (!defined(BOOLECTOR) && !defined(BOOLECTOR_OLD) && defined(Z3))
    _Factory = LSolver::ExpFactory(new UrsaMajor::BVExpressionFactoryZ3());
    _BV = true;
    break;
#endif
  }

  _ExpToAddIntoSolver = aExp::TOP();
}

LSolver::~LSolver() {}

void LSolver::PrepareNoAck(caExp &a, caExp &b, aExp &abs_a, aExp &abs_b,
                           aExp &abs_neg_b) {

  saExp eqs;
  LAckermannization::SimplifyLeftRight(a, eqs);
  LAckermannization::SimplifyLeftRight(b, eqs);
  abs_a = lav::AddEqs(a, eqs);
  abs_b = b;
  abs_neg_b = aExp::NOT(abs_b);

}

void LSolver::PrepareAck(caExp &a, caExp &b, aExp &abs_a, aExp &abs_b,
                         aExp &abs_neg_b, saExp &ls, saExp &rs) {
  aExp r_a = a, r_b = b;
  //  AckermannizeTimer.startTimer();
  LAckermannization::Ackermannize(r_a, r_b, ls, rs);
  //  AckermannizeTimer.stopTimer();

  abs_a = r_a;
  abs_b = r_b;
  abs_neg_b = aExp::NOT(abs_b);
}

thread_local std::map<std::string, long> LSolver::_Addresses;
thread_local std::map<aExp, UrsaExp> LSolver::_AbstractLA;

UrsaExp LSolver::AbstrAck(caExp &e, stUrsaExp &symbolTable, cStr &abs) {
  if (abs == LEFT)
    return ExportVariable(e, symbolTable, LEFT);
  else
    return ExportVariable(e, symbolTable, RIGHT);
}

UrsaExp LSolver::GetImplication(caExp &e1, caExp &e2, cStr &abs,
                                std::map<aExp, aExp> &ackselects,
                                stUrsaExp &symbolTable) {
  UrsaExp impl;

  if ((e1.isSelect()) || (e2.isSelect())) {
    aExp select, nselect;
    //FIXME ovo je samo za jedan
    if (e1.isSelect()) {
      select = e1;
      nselect = e2;
    } else {
      select = e2;
      nselect = e1;
    }

    aExp e;
    if (ackselects.find(select) != ackselects.end())
      e = ackselects[select];
    else {
      std::string name = GetNextVariable();
      //FIXME tip
      e = ExpVar(name, fpointer_type, false);
      ackselects.insert(std::pair<aExp, aExp>(select, e));
    }
    UrsaExp e1;
    bool exported = TryExportExpression(select, e1, symbolTable);
    if (!exported)
      return UrsaExp::booleanGround(true);
    UrsaExp eq1 = ExportVariable(nselect, symbolTable) == e1;
    UrsaExp eq2 =
        (AbstrAck(nselect, symbolTable, abs) == AbstrAck(e, symbolTable, abs));
    impl = !eq1 || eq2;

  } else {
    UrsaExp e11;
    bool exported = TryExportExpression(e1, e11, symbolTable);
    if (!exported)
      return UrsaExp::booleanGround(true);

    UrsaExp e22;
    exported = TryExportExpression(e2, e22, symbolTable);
    if (!exported)
      return UrsaExp::booleanGround(true);
    UrsaExp eq1 = e11 == e22;
    UrsaExp v1 = AbstrAck(e1, symbolTable, abs);
    UrsaExp v2 = AbstrAck(e2, symbolTable, abs);
    UrsaExp eq2 = (v1 == v2);
    impl = !eq1 || eq2;
  }

  return impl;
}

void LSolver::AckImplicationsForTwoSets(const saExp &ls, const saExp &rs,
                                        UrsaExp &result_e,
                                        stUrsaExp &symbolTable) {
  vaExp lefts;
  vaExp rights;
  vaExp Lefts;
  vaExp Rights;
  UrsaExp implications = UrsaExp::booleanGround(true);

  CopySetIntoVector(ls, lefts);
  CopySetIntoVector(rs, rights);

  CopySetIntoVector(_Lefts, Lefts);
  CopySetIntoVector(_Rights, Rights);

  std::map<aExp, aExp> ackselects;

  for (unsigned i = 0; i < Lefts.size(); i++)
    for (unsigned j = 0; j < lefts.size(); j++) {
      if (isAddress(Lefts[i].GetName()) && isAddress(lefts[j].GetName()))
        continue;
      implications = implications && GetImplication(Lefts[i], lefts[j], LEFT,
                                                    ackselects, symbolTable);
    }

  for (unsigned i = 0; i < Rights.size(); i++)
    for (unsigned j = 0; j < rights.size(); j++) {
      if (isAddress(Rights[i].GetName()) && isAddress(rights[j].GetName()))
        continue;
      implications = implications && GetImplication(Rights[i], rights[j], RIGHT,
                                                    ackselects, symbolTable);
    }
  result_e = implications && result_e;
}

void LSolver::AckImplications(const saExp &ls, const saExp &rs,
                              UrsaExp &result_e, stUrsaExp &symbolTable) {
  vaExp lefts;
  vaExp rights;

  UrsaExp implications = UrsaExp::booleanGround(true);
  UrsaExp getimp = UrsaExp::booleanGround(true);
  unsigned b = 0;

  CopySetIntoVector(ls, lefts);
  CopySetIntoVector(rs, rights);

  std::map<aExp, aExp> ackselects;
  for (unsigned i = 0; (lefts.size() > 0) && i < lefts.size() - 1; i++)
    for (unsigned j = i + 1; j < lefts.size(); j++) {
      if (isAddress(lefts[i].GetName()) && isAddress(lefts[j].GetName()))
        continue;
      implications = implications && GetImplication(lefts[i], lefts[j], LEFT,
                                                    ackselects, symbolTable);
      b++;
    }

  for (unsigned i = 0; (rights.size() > 0) && i < rights.size() - 1; i++)
    for (unsigned j = i + 1; j < rights.size(); j++) {
      if (isAddress(rights[i].GetName()) && isAddress(rights[j].GetName()))
        continue;
      implications = implications && GetImplication(rights[i], rights[j], RIGHT,
                                                    ackselects, symbolTable);
      b++;
    }

  if (b == 0) { //FIXME nije jasno cemu ovo sluzi i da li bi se moglo izostaviti
                //--- na test1 deluje da moze, treba proveriti i ostale
    if (ls.size() == 1)
      implications =
          implications && (AbstrAck(*(ls.begin()), symbolTable, LEFT) ==
                           AbstrAck(*(ls.begin()), symbolTable, LEFT));
    if (rs.size() == 1)
      implications =
          implications && (AbstrAck(*(rs.begin()), symbolTable, RIGHT) ==
                           AbstrAck(*(rs.begin()), symbolTable, RIGHT));
  }

  result_e = implications && result_e;
}

void LSolver::Prepare(caExp &a, caExp &b, aExp &abs_a, aExp &abs_b,
                      aExp &abs_neg_b, saExp &ls, saExp &rs) {

  switch (solver) {
  case Z3LA:
  case Z3BV:
  case YicesLA:
  case YicesBV:
  case MSLA:
  case MSBV:
  case BoolectorBVEUF:
    PrepareNoAck(a, b, abs_a, abs_b, abs_neg_b);
    break;
  case BoolectorBV:
  case Z3LAACK:
  case Z3BVACK:
  case YicesLAACK:
  case YicesBVACK:
  case MSLAACK:
  case MSBVACK:
    PrepareAck(a, b, abs_a, abs_b, abs_neg_b, ls, rs);
    break;
  default:
    PrepareAck(a, b, abs_a, abs_b, abs_neg_b, ls, rs);
  }

}
bool LSolver::Ackermannize() {
  if ((solver == Z3BVACK) || (solver == BoolectorBV) || (solver == Z3LAACK) ||
      (solver == YicesLAACK) || (solver == MSLAACK) || (solver == YicesBVACK) ||
      (solver == MSBVACK))
    return true;
  else
    return false;
}

bool LSolver::HasArrays() {
  if ((solver == Z3BVACK) || (solver == BoolectorBV) ||
      (solver == BoolectorBVEUF) || (solver == Z3LAACK) || (solver == Z3BV) ||
      (solver == Z3LA))
    return true;
  else
    return false;
}

void LSolver::PrepareSolver() {
#if defined(YICES)
  if (isYices()) {
    //FIXME proveriti da li ovo .clear treba i za ostale!!!
    _AbstractLA.clear();
    UrsaMajor::YicesInstance::instance().del();
  }
#endif
#if defined(MATHSAT)
  if (isMathSAT()) {
    //FIXME proveriti da li ovo .clear treba i za ostale!!!
    _AbstractLA.clear();
    UrsaMajor::MathSATInstance::instance().reset();
  }
#endif
}

STATUS LSolver::callSolver(caExp &a, caExp &b) {
  return callSolver(a, b, NULL, NULL, OTHER, false, BRANCHING);
}

bool LSolver::GetOut(SOLVERCONTEXT c, bool satnegb) {

  //ako se samo proverava reachability, onda je dovoljna jedna provera
  if (c == REACHABILITY)
    return true;
  //ako se proverava branching onda ako je false nema potrebe za drugom proverom
  if (c == BRANCHING) {
    if (satnegb == false)
      return true;
    else
      return false;
  }
  if (light)
    return true;
  //ako ne pratim unreachable, onda nam je sve jedno da li je safe
  //ili unreachable ako pratimo unreachable, onda cemo uraditi i
  //drugu proveru da bi znali da li je stvarno safe ili je
  //mozda unreachable
  if (!unreachable) {
    if (satnegb == false)
      return true;
    else
      return false;
  }

  return false;
}

STATUS LSolver::callSolver(caExp &a, caExp &b, const LBlock *fb,
                           const LInstruction *fi, ERRKIND erKind, bool m,
                           SOLVERCONTEXT c) {

//      std::cout << "---------------------------------------------------"
//                << std::endl;
//      std::cout << "callSolver" << std::endl;
//      std::cout << "---------------------------------------------------"
//                << std::endl;
//      PrintAB(a, b);

  //  NonIncrementalPreparationTime.startTimer();
  saExp ls;
  saExp rs;

  aExp abs_a, abs_b, abs_neg_b;
  Prepare(a, b, abs_a, abs_b, abs_neg_b, ls, rs);

  UrsaExp::setFactory(_Factory.get());
  PrepareSolver();

  UrsaExp exported_a, exported_b, exported_neg_b;
  stUrsaExp symbolTable;

  bool exported;
//  PrintA(abs_a, "abs_a");
  exported = TryExportExpression(abs_a, exported_a, symbolTable);
  if (!exported)
    return ERROR;
//  std::cout << "callSolver exported_a" << std::endl;
//  std::cout << exported_a << std::endl;
//  std::cout << "symbolTable" << std::endl;
//  symbolTable.print(std::cout);

//  PrintA(abs_neg_b, "callSolver abs_neg_b");

//  exported = TryExportExpression(abs_neg_b, exported_neg_b, symbolTable, erKind);

  if(isOverflow(erKind) || isUnderflow(erKind)) {

      UrsaExp exported_b0, exported_b1;
      aExp b = abs_b;
      bool exportedb0 = TryExportExpression(b[0], exported_b0, symbolTable, erKind);
      bool exportedb1 = TryExportExpression(b[1], exported_b1, symbolTable, erKind);

      if (b.isAdd() && (erKind == OVERFLOW_ADD))
           exported_b = exported_b0.addOverflow(exported_b1);
      else if (b.isAdd() && (erKind == UNDERFLOW_ADD))
           exported_b = exported_b0.addUnderflow(exported_b1);
      else if (b.isSub() && (erKind == OVERFLOW_SUB))
          exported_b = exported_b0.subOverflow(exported_b1);
      else if (b.isSub() && (erKind == UNDERFLOW_SUB))
          exported_b = exported_b0.subOverflow(exported_b1);
      else if (b.isMul() && (erKind == OVERFLOW_MUL))
          exported_b = exported_b0.mulOverflow(exported_b1);
      else if (b.isMul() && (erKind == UNDERFLOW_MUL))
          exported_b = exported_b0.mulUnderflow(exported_b1);
      else if (b.isUdiv() && (erKind == OVERFLOW_UDIV))
          exported_b = exported_b0.udivOverflow(exported_b1);
      else if (b.isSdiv() && (erKind == OVERFLOW_SDIV))
          exported_b = exported_b0.sdivOverflow(exported_b1);
        else {std::cout << " PROBLEM " << std::endl;}
      exported = exportedb0 && exportedb1;
      exported_neg_b = !exported_b;
  }
  else  {
      exported = TryExportExpression(abs_b, exported_b, symbolTable, erKind);
      exported_neg_b = !exported_b;
  }


  if (!exported) {
      std::cout << "!exported" << std::endl;
    return ERROR;
  }
/*  std::cout << "exported_b" << std::endl;
  std::cout << exported_b << std::endl;
  std::cout << "exported_neg_b" << std::endl;
  std::cout << exported_neg_b << std::endl;
  std::cout << "symbolTable" << std::endl;
  symbolTable.print(std::cout);*/

  //Posle exporta, dodati ackimplikacije, ako je potrebno
  if (Ackermannize())
    AckImplications(ls, rs, exported_a, symbolTable);

  //  NonIncrementalPreparationTime.stopTimer();
  //  NonIncrementalTime.startTimer();

  AddConstraint(exported_a);
  bool satnegb = AddTempConstraint(exported_neg_b);
  //  NonIncrementalTime.stopTimer();

  if (m && (c == NORMAL) && (satnegb == true) && Model) {
    std::cout << "GetModel" << std::endl;
    GetModel(symbolTable, fb, fi, erKind);
  }

  if (GetOut(c, satnegb)) {
    //    ResetTime.startTimer();
    resetSolver();
    //   ResetTime.stopTimer();
    if (satnegb == false)
      return SAFE;
    else
      return UNSAFE;
  }

//  exported = TryExportExpression(abs_b, exported_b, symbolTable, erKind);
//  if (!exported)
//    return ERROR;

  //  NonIncrementalTime.startTimer();
  bool satb = AddTempConstraint(exported_b);
  //  NonIncrementalTime.stopTimer();

  //  ResetTime.startTimer();
  resetSolver();
  //  ResetTime.stopTimer();

  return GetStatus(((satnegb == true) ? SAT : UNSAT),
                   ((satb == true) ? SAT : UNSAT));
}

STATUS LSolver::callSolver(caExp &a) {
  //da bi ovo moglo onda bi trebalo ukljuciti jos jedan parametar ili ono br
  //prebaciti da bude kontekst
  //pa da se zna da li je treba i u kom slucaju da se staje
  return callSolver(aExp::TOP(), aExp::NOT(a), NULL, NULL, OTHER, false,
                    REACHABILITY);
}

void LSolver::GetFunctionTransforms(const vStr &transformations, cStr &fname,
                                    vStr &transforms, cStr &context) {
  bool b = false;

  for (unsigned un = 0; un < transformations.size(); un++) {

    if (StartsWith(transformations[un], fname)) {
      b = true;
      if (context != "") {
        if (EndsWith(transformations[un], context)) {
          std::string tr =
              cutend(cut(transformations[un], (fname + "_")), context);
          if (tr.find(CONTEXT) != std::string::npos)
            continue;
          transforms.push_back(tr);
        }
      } else
        transforms.push_back(cut(transformations[un], (fname + "_")));
      continue;
    }
    if (b)
      break;
  }
}

void LSolver::GetEdges(const vStr &transforms, std::vector<unsigned> &prvi,
                       std::vector<unsigned> &drugi) {
  prvi.resize(transforms.size());
  drugi.resize(transforms.size());
  for (unsigned u = 0; u < transforms.size(); u++) {
    prvi[u] = drugi[u] = 0;
    std::string s(transforms[u]);
    unsigned i = 0;
    for (; i < s.size(); i++) {
      if (s[i] == '_')
        break;
      prvi[u] *= 10;
      prvi[u] += (s[i] - '0');
    }
    i++;
    for (; i < s.size(); i++) {
      if ((s[i] != '0') && (s[i] != '1') && (s[i] != '2') && (s[i] != '3') &&
          (s[i] != '4') && (s[i] != '5') && (s[i] != '6') && (s[i] != '7') &&
          (s[i] != '8') && (s[i] != '9'))
        break;
      drugi[u] *= 10;
      drugi[u] += (s[i] - '0');
    }
  }

}

void LSolver::WriteStoreValues(std::ostream &ostr, const LBlock *fb_tekuci,
                               stUrsaExp &symbolTable, std::string s) {

  std::set<std::string> skip;
  mspaExp modelValues = fb_tekuci->GetModelValues();

  mspaExp::const_iterator it = modelValues.begin(), ite = modelValues.end();
  for (; it != ite; it++) {
    UrsaExp exp;
    bool b;

    std::string name;
    if (s == "") {
      b = TryExportExpression(*(it->second), exp, symbolTable);
      if (!b)
        return;
    } else {
      std::string name = it->second->GetName() + s;
      if (symbolTable.contains(name))
        exp = symbolTable.get(name);
      else
        continue;
    }

    std::string asgn;
    try {
      asgn = exp.getAssignment();
    }
    catch (char const * ss) {
      ostr << it->first << "  " << ss << std::endl;
      continue;
    }
    if (isBoolector() && asgn[0] == 'x') {
      //ostr << it->first << " = " /*<<std::flush << s << std::endl */<< "any
      //value" << ", " ;
    } else
      //TODO asgn ne mora da bude broj
      //ukoliko je u pitanju niz, treba izvuci odgovarajuce informacije
      //da bi ovde omogucili da se dobije niz, potrebno je u
      //LState::GetModelValues() dozvoliti da se u tabelu ubace nizovi 
      //sto je trenutno zabranjeno jer nema podrske za nizove
      ostr << it->first << " = " /*<<std::flush << s << std::endl */
           << binarystring2signed(asgn) << ", ";
  }
  ostr << std::endl;
}

/*
Ako hoces da proveris da su sve tri safe, treba da ovo bude valjano
F => c1 and c2 and c3
tj. ovo da bude nezadovoljivo:
not (F => c1 and c2 and c3)
tj.
F and not (c1 and c2 and c3)
tj.
F and (not c1 or not c2 or not c3)
treba da bude nezadovoljivo

ovo c1, c2, c3 moze da bude iskazno slovo p1,p2,p3 koje definise uslov:
F and (not p1 or not p2 or not p3) and
(p1 <=> c1) and (p2 <=> c2) and (p3 <=> c3)

ili
F and (p1 or p2 or p3) and
(p1 <=> not c1) and (p2 <=> not c2) and (p3 <=> not c3)

ili
F and (p1 or p2 or p3) and
(p1 <=> a1/\!b1) and (p2 <=> a2/\!b2) and (p3 <=> a3/\!b3)

Ako je formula zadovoljiva, onda neka formula nije safe.
Ako je formula zadovoljiva, onda je (not p1 or not p2 or not p3) tacno, recimo
not p1 je tacno (ti pitas model za sva tri: p1, p2, p3), pa prva komanda nije
safe. Ako je jos neki od not p2, not p3 tacno, onda ni ta komanda nije safe.*/

STATUS LSolver::callSolverBlock(caExp &f,
                                std::vector<LLocalCondition *> &conds) {

//      std::cout << pthread_self() << " ---------------"<< std::endl;
//      std::cout << pthread_self() << " callSolverBLOCK"<< std::endl;
//      std::cout << pthread_self() << " conds.size() " << conds.size() << std::endl;
//      std::cout << pthread_self() << " ---------------"<< std::endl;

  if (conds.size() == 0)
    return SAFE;

  UrsaExp::setFactory(_Factory.get());

  if (FinalAddIntoSolver() == false)
    return ERROR;

  /*  if (conds.size() == 1) {
        STATUS s =
          callSolverIncremental(aExp::AND(f, conds[0]->LHS()), conds[0]->RHS(),
                                conds[0]->Instruction()->GetParentBlock(),
                                conds[0]->Instruction(), conds[0]->ErrorKind());
      conds[0]->Status() = s;
      return s;
    }*/


  //uraditi prvo varijantu bez akermanizacije
  //F and (p1 or p2 or p3) and
  //(p1 <=> a1/\!b1) and (p2 <=> a2/\!b2) and (p3 <=> a3/\!b3)
  std::vector<aExp> pis;
  std::vector<aExp> ands;

  for (unsigned i = 0; i < conds.size(); i++) {
    aExp pi = ExpPropVar;
    pis.push_back(pi);
    aExp anegb = aExp::AND(conds[i]->LHS(), aExp::NOT(conds[i]->RHS()));
    ands.push_back(aExp::IFF(pi, anegb));
  }

  ands.push_back(aExp::OR(pis));
  ands.push_back(f);
  aExp blockCond = aExp::AND(ands);

  UrsaExp exported_cond;
  bool exported;

  exported = TryExportExpression(blockCond, exported_cond, _SymbolTable);
  if (!exported)
    return ERROR;

  //proverava se negacija uslova ispravnosti
  bool satSafety = AddTempConstraint(exported_cond);

  //negacija uslova ispravnosti je nezadovoljiva sve komane su safe
  if (satSafety == false) {
    //postavlja se svima status na safe
    for (unsigned i = 0; i < conds.size(); i++)
      conds[i]->Status() = SAFE;
    return SAFE;
  }

  for (unsigned i = 0; i < pis.size(); i++) {
    UrsaExp exp;
    bool b;
    b = TryExportExpression(pis[i], exp, _SymbolTable);
    if (!b)
      return ERROR;
    std::string s;
    try {
      s = exp.getAssignment();
    }
    catch (char const * ss) {
      std::cout << "callSolverBlock::exception  " << ss << std::endl;
      continue;
    }
    if (s == "0")
      conds[i]->Status() = BLOCK_UNKNOWN;
    if (s == "1")
      conds[i]->Status() = UNSAFE;
  }

  return UNSAFE;
}

//vraca true ako je uspelo dodavanje
bool LSolver::FinalAddIntoSolver() {
  saExp ls;
  saExp rs;
  aExp abs_cond;
  if (Ackermannize()) {
    //ovim se dobije flattened abs_cond
    //    GlobalAckermannizationTimer.startTimer();
    LAckermannization::GlobalAckermannization(_ExpToAddIntoSolver, abs_cond, ls,
                                              rs, _Acks);
    //    GlobalAckermannizationTimer.stopTimer();
  } else {
    //NOVO
    saExp eqs;
    LAckermannization::SimplifyLeftRight(_ExpToAddIntoSolver, eqs);
    abs_cond = lav::AddEqs(_ExpToAddIntoSolver, eqs);
    //      abs_cond = _ExpToAddIntoSolver;
  }

  //  PrintA(_ExpToAddIntoSolver, " AddIntoSolver ");
  //  PrintA(abs_cond, " a-------------- ");

  UrsaExp exported_cond;
  bool exported;
  exported = TryExportExpression(abs_cond, exported_cond, _SymbolTable);
  if (!exported)
    return false;

  _ExpToAddIntoSolver = aExp::TOP();

  //Posle exporta, dodati ackimplikacije, ako je potrebno
  if (Ackermannize()) {
    AckImplications(ls, rs, exported_cond, _SymbolTable);
    AckImplicationsForTwoSets(ls, rs, exported_cond, _SymbolTable);
    SetUnion(_Lefts, ls);
    SetUnion(_Rights, rs);
  }

  //  IncrementalTime.startTimer();
  AddConstraint(exported_cond);
  //  IncrementalTime.stopTimer();
  return true; //uspelo dodavanje
}

void LSolver::GetConstraints(caExp &a, caExp &b, aExp &abs_a, aExp &abs_b, aExp &abs_neg_b, //aExp &ab, aExp &anegb,
                             saExp &ls, saExp &rs) {
  if (Ackermannize()) {
//    aExp abs_a, abs_b, abs_neg_b;
    PrepareAckIncremental(a, b, abs_a, abs_b, abs_neg_b, ls, rs);
//    ab = aExp::AND(abs_a, abs_b);
//    anegb = aExp::AND(abs_a, abs_neg_b);
  } else {
//    aExp abs_a, abs_b, abs_neg_b;
    PrepareNoAck(a, b, abs_a, abs_b, abs_neg_b);
//    ab = aExp::AND(abs_a, abs_b);
//    anegb = aExp::AND(abs_a, abs_neg_b);
  }
}


bool LSolver::ExportOU(caExp &b, UrsaExp &exported_b, ERRKIND erKind, stUrsaExp& symbolTable) {
    if(isOverflow(erKind) || isUnderflow(erKind)) {
        UrsaExp exported_b0, exported_b1;
        bool exportedb0 = TryExportExpression(b[0], exported_b0, symbolTable, OTHER);
        bool exportedb1 = TryExportExpression(b[1], exported_b1, symbolTable, OTHER);

        if (b.isAdd() && (erKind == OVERFLOW_ADD))
             exported_b = exported_b0.addOverflow(exported_b1);
        if (b.isAdd() && (erKind == UNDERFLOW_ADD))
             exported_b = exported_b0.addUnderflow(exported_b1);
        if (b.isSub() && (erKind == OVERFLOW_SUB))
            exported_b = exported_b0.subOverflow(exported_b1);
        if (b.isSub() && (erKind == UNDERFLOW_SUB))
            exported_b = exported_b0.subOverflow(exported_b1);
        if (b.isMul() && (erKind == OVERFLOW_MUL))
            exported_b = exported_b0.mulOverflow(exported_b1);
        if (b.isMul() && (erKind == UNDERFLOW_MUL))
            exported_b = exported_b0.mulUnderflow(exported_b1);
        if (b.isUdiv() && (erKind == OVERFLOW_UDIV))
            exported_b = exported_b0.udivOverflow(exported_b1);
        if (b.isSdiv() && (erKind == OVERFLOW_SDIV))
            exported_b = exported_b0.sdivOverflow(exported_b1);

        return exportedb0 && exportedb1;
    }
    else return false;

}

bool LSolver::ExportOU(caExp &b, UrsaExp &exported_b, ERRKIND erKind) {
    return ExportOU(b,exported_b,erKind, _SymbolTable);
}

bool LSolver::ExportOU(caExp &a, caExp &negb, UrsaExp &exported_anegb, ERRKIND erKind) {
    UrsaExp exported_a, exported_negb, exported_b;
    bool exportedb, exported;
    bool exporteda = TryExportExpression(a, exported_a, _SymbolTable, OTHER);

    aExp b;
    if(negb.IsNOT()) b = negb[0];
    else b = negb;

    exportedb = ExportOU(b,exported_b,erKind);

    if(negb.IsNOT()) exported_negb = !exported_b;
    else exported_negb = exported_b;

    exported_anegb = exported_a && exported_negb;
    exported = exporteda && exportedb;

    return exported;
}
bool LSolver::Export(caExp &a, caExp &negb, UrsaExp &exported_anegb, saExp &ls,
                     saExp &rs, UrsaExp &impls, ERRKIND erKind) {
  bool exported;

  if(isOverflow(erKind) || isUnderflow(erKind)) {
    exported = ExportOU(a, negb, exported_anegb, erKind);
  } else {
      UrsaExp exported_a, exported_negb;
      bool exporteda = TryExportExpression(a, exported_a, _SymbolTable, erKind);
      bool exportednegb = TryExportExpression(negb, exported_negb, _SymbolTable, erKind);
      exported_anegb = exported_a && exported_negb;
      exported = exporteda && exportednegb;
  }
  if (!exported)
    return false;

  impls = UrsaExp::booleanGround(true);
  if (Ackermannize()) {
    AckImplications(ls, rs, impls, _SymbolTable);
    AckImplicationsForTwoSets(ls, rs, impls, _SymbolTable);
    exported_anegb = impls && exported_anegb;
  }

  return true;
}

bool LSolver::Export(caExp &a, caExp& b, UrsaExp &exported_ab, UrsaExp &impls) {
  bool exporteda, exportedb;
  UrsaExp exported_a, exported_b;
  exporteda = TryExportExpression(a, exported_a, _SymbolTable);
  exportedb = TryExportExpression(b, exported_b, _SymbolTable);
  if (!(exporteda && exportedb))
    return false;
  exported_ab = exported_a && exported_b;
  if (Ackermannize()) {
    exported_ab = impls && exported_ab;
  }

  return true;
}

STATUS LSolver::callSolverIncremental(caExp &a, caExp &b, const LBlock *fb,
                                      const LInstruction *fi, ERRKIND erKind) {

//    std::cout << "\n----------------------------------------------------" << std::endl;
//    std::cout << "callSolverIncremental" << std::endl;
//    PrintAB(a, b);

  UrsaExp::setFactory(_Factory.get());

  if (FinalAddIntoSolver() == false)
    return ERROR;

  saExp ls;
  saExp rs;

  //anegb je negacija uslova ispravnosti
  //ab je negacija uslova neispravnosti
  aExp ab, anegb;
  aExp abs_a, abs_b, abs_neg_b;
  GetConstraints(a, b, abs_a, abs_b, abs_neg_b, ls, rs);
  ab = aExp::AND(abs_a, abs_b);
  anegb = aExp::AND(abs_a, abs_neg_b);

//  UrsaExp::setFactory(_Factory.get());
  UrsaExp exported_anegb, exported_ab, impls;
  if (Export(abs_a, abs_neg_b, exported_anegb, ls, rs, impls, erKind) == false)
    return ERROR;

  //proverava se negacija uslova ispravnosti
  //  IncrementalTime.startTimer();
  bool satnegb = AddTempConstraint(exported_anegb);
  //  IncrementalTime.stopTimer();

  //negacija uslova ispravnosti je nezadovoljiva komanda je safe
  if (satnegb == false && !unreachable) {
    return SAFE;
  }

  //zadovoljiva je, znaci da je ili unsafe ili flawed
  if (Model && (satnegb == true)) {
    GetModel(_SymbolTable, fb, fi, erKind);
  }

  if (light) {
    if (satnegb == true)
      return UNSAFE;
    else
      return SAFE;
  }

  if (Export(abs_a, abs_b, exported_ab, ls, rs, impls, erKind) == false)
    return ERROR;

  //  IncrementalTime.startTimer();
  bool satb = AddTempConstraint(exported_ab);
  //  IncrementalTime.stopTimer();

  return GetStatus(((satnegb == true) ? SAT : UNSAT),
                   ((satb == true) ? SAT : UNSAT));
}

bool LSolver::TryExportExpression(caExp &a, UrsaExp &exported_a,
                                  stUrsaExp &symbolTable, ERRKIND erKind) {
    try {
    if (_BV) {
      exported_a = ExportExpressionBV(a, symbolTable, erKind);
    }
      else
      exported_a = ExportExpressionLA(a, symbolTable);
  }
  catch (const char * msg) {
    std::cout << " callSolver::export exception " << msg << std::endl;
    return false;
  }

  return true;
}

//Ovo je po default-u
#if defined(BOOLECTOR) || defined(BOOLECTOR_OLD)
thread_local LSolver::ExpFactory LSolver::_Factory =
    LSolver::ExpFactory(new UrsaMajor::BVExpressionFactoryBoolector());
#endif

#if (!defined(BOOLECTOR) && !defined(BOOLECTOR_OLD) && defined(Z3))
thread_local LSolver::ExpFactory LSolver::_Factory =
    LSolver::ExpFactory(new UrsaMajor::BVExpressionFactoryZ3());
#endif

#if (!defined(BOOLECTOR) && !defined(BOOLECTOR_OLD) && !defined(Z3) &&         \
     defined(YICES))
thread_local LSolver::ExpFactory LSolver::_Factory =
    LSolver::ExpFactory(new UrsaMajor::BVExpressionFactoryYices());
#endif

#if (!defined(BOOLECTOR) && !defined(BOOLECTOR_OLD) && !defined(Z3) &&         \
     !defined(YICES) && defined(MATHSAT))
thread_local LSolver::ExpFactory LSolver::_Factory =
    LSolver::ExpFactory(new UrsaMajor::BVExpressionFactoryMathSAT());
#endif

#if (!defined(BOOLECTOR) && !defined(BOOLECTOR_OLD) && !defined(Z3) &&         \
     !defined(YICES) && !defined(MATHSAT))
thread_local LSolver::ExpFactory LSolver::_Factory = LSolver::ExpFactory(0);
#endif

thread_local bool LSolver::_BV = true;

//ovim se ne dodaje stvarno vec se samo pamti sta treba da se doda
STATUS LSolver::AddIntoSolver(caExp &cond) {
  _ExpToAddIntoSolver = aExp::AND(_ExpToAddIntoSolver, cond);
  return SAFE; //u stvari reachable
}

void LSolver::PrepareAckIncremental(caExp &a, caExp &b, aExp &abs_a,
                                    aExp &abs_b, aExp &abs_neg_b, saExp &ls,
                                    saExp &rs) {

  //  GlobalAckermannizationTimer.startTimer();
  LAckermannization::GlobalAckermannization(a, b, abs_a, abs_b, ls, rs, _Acks);
  //  GlobalAckermannizationTimer.stopTimer();

  abs_neg_b = aExp::NOT(abs_b);

}

bool LSolver::AddTempConstraint(UrsaExp &e) {
  //    std::cout << "LSolver::addTempConstraint()" <<std::endl;
  bool sat = e.addTempConstraint();
  //    std::cout << ((sat==true) ? "sat":"unsat") <<std::endl;
  return sat;
}

bool LSolver::AddConstraint(UrsaExp &e) {
  //std::cout << "LSolver::addConstraint()" <<std::endl;
  bool sat = e.addConstraint();
  return sat;
}

//VAZNE NAPOMENE:
//
//da bi se cistila tabela mora da se brise kontekst a ne da se resetuje jer
//resetovanje ne brise promenljive koje su vec napravljene vec samo izbacuje
//tvrdjenje koje je proveravano

//da bi se brisao kontekst potrebno je da se uradi brisanje uvek pre poziva
//exportovanja
//problem je sto za yices bv to radi a za yices la ne radi i zato sam od toga
//odustala
//u oba slucaja radi za mathsat
//za boolector je neophodno da se brise tabela jer kod njega nema resetovanja
//konteksta vec samo brisanje
//ako se resetuje kontekst, neophodno je da je predhodno pozivan dokazivac, ako
//nije program puca

//Ako ne uspe exportovanje, onda moze da ne bude resetovan kontekst
//to za boolektor i mathsat nije ni vazno
//za yices mozda jeste vazno, ali tu se symbol table ionako ne brise pa valjda
//ni tu nije vazno

} //end of namespace
