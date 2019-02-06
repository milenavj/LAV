//===-- LSolver.h ---------------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LSOLVER_H
#define LSOLVER_H

#include "lav/Misc/misc.h"
#include "expression/expressions/Expression.h"
#include "solvers/solver-interfaces/expression.hpp"
#include "solvers/syntax/symbol-table.hpp"

namespace argo { class Expression; }

namespace lav {
class LBlock;
class LInstruction;
class LLocalCondition;

typedef UrsaMajor::Expression UrsaExp;
typedef UrsaMajor::SymbolTable<UrsaExp> stUrsaExp;

class LSolver {
  typedef enum satisfiability {
    UNSAT,
    SAT,
    UNKNOWN
  } SATISFIABILITY;
  typedef enum context {
    NORMAL,
    BRANCHING,
    REACHABILITY
  } SOLVERCONTEXT;
  typedef std::auto_ptr<UrsaMajor::ExpressionFactory> ExpFactory;

public:

  static LSolver &instance() {
    thread_local static LSolver _instance;
    return _instance;
  }

  static STATUS callSolver(caExp &a);
  static STATUS callSolver(caExp &a, caExp &b);
  static STATUS callSolver(caExp &a, caExp &b, const LBlock *fb,
                           const LInstruction *fi, ERRKIND erKind,
                           bool m = false, SOLVERCONTEXT c = NORMAL);
  static STATUS GetStatus(SATISFIABILITY sat1, SATISFIABILITY sat2);

  STATUS callSolverBlock(caExp &f, std::vector<LLocalCondition *> &ci);
  STATUS callSolverIncremental(caExp &a, caExp &b, const LBlock *fb,
                               const LInstruction *fi, ERRKIND erKind);
  STATUS AddIntoSolver(caExp &cond);
  static void SetAddresses(caExp &addr);
  void reset();
  static void resetSolver();
  static void setFactory();
  static void GetModel(stUrsaExp &symbolTable, const LBlock *fb,
                       const LInstruction *fi, ERRKIND erKind);
  static void GetTransform(stUrsaExp &symbolTable, vStr &transforamtions);
  static void GetFunctionTransforms(const vStr &transformations, cStr &fname,
                                    vStr &transforms, cStr &context = "");
  static void GetEdges(const vStr &transforms, vUnsigned &prvi,
                       vUnsigned &drugi);

  static void WriteMergedBlocks(std::ostream &f, const LBlock *fb,
                                stUrsaExp &symbolTable,
                                const LBlock *fb_limit = NULL);
  static void WriteStoreValues(std::ostream &ostr, const LBlock *fb_tekuci,
                               stUrsaExp &symbolTable, std::string s = "");
  static void WriteBlockInfo(std::ostream &ostr, stUrsaExp &symbolTable,
                             const LBlock *fb, const LInstruction *fi = NULL,
                             std::string s = "");
  static void WriteTrace(std::ostream &f, vUnsigned &prvi, vUnsigned &drugi,
                         const LBlock *fb_tekuci, stUrsaExp &symbolTable);
  static bool isLATheory();

private:
  LSolver();
  ~LSolver();

  static void PrepareSolver();
  static aExp AbstractFormulaVariable(caExp &e);
  static aExp AbstractVariable(caExp &e);
  static std::string AbstractName(caExp &e);

  static aExp RenameExpressionVariables(caExp &e, int Context,
                                        cStr &FunctionName);
  static aExp ReplaceAddresses(caExp &e,
                               std::map<std::string, long> &Addresses);
  static void GetAddresses(caExp &addr, std::map<std::string, long> &Addresses);
  static bool TryExportExpression(caExp &a, UrsaExp &exported_a,
                                  stUrsaExp &symbolTable, ERRKIND erKind = OTHER);
  static UrsaExp ExportExpressionBV(caExp &e, stUrsaExp &symbolTable, ERRKIND erKind);
  static UrsaExp ExportExpressionLA(caExp &e, stUrsaExp &symbolTable);
  static UrsaExp ExportVariable(caExp &e, stUrsaExp &symbolTable, cStr &s = "");
  static bool AddTempConstraint(UrsaExp &e);
  static bool AddConstraint(UrsaExp &e);

  bool FinalAddIntoSolver();
  void GetConstraints(caExp &a, caExp &b, aExp &abs_a, aExp &abs_b, aExp &abs_neg_b, //aExp &ab, aExp &anegb,
                      std::set<aExp> &ls, std::set<aExp> &rs);
  bool Export(caExp &a, caExp& b, UrsaExp &ue, std::set<aExp> &ls, std::set<aExp> &rs,
              UrsaExp &impls, ERRKIND erKind);
  bool ExportOU(caExp &a, caExp& b, UrsaExp &ue, ERRKIND erKind);
  bool ExportOU(caExp &b, UrsaExp &exported_b, ERRKIND erKind);
  static bool ExportOU(caExp &b, UrsaExp &exported_b, ERRKIND erKind, stUrsaExp &symbolTable);
  bool Export(caExp &a, caExp& b, UrsaExp &ue, UrsaExp &impls);

private:
  static bool GetOut(SOLVERCONTEXT c, bool satnegb);
  static bool isMathSAT();
  static bool isZ3();
  static bool isBoolector();
  static bool isYices();

  static void AckImplications(const std::set<aExp> &ls,
                              const std::set<aExp> &rs, UrsaExp &e,
                              stUrsaExp &symbolTable);
  void AckImplicationsForTwoSets(const std::set<aExp> &ls,
                                 const std::set<aExp> &rs, UrsaExp &result_e,
                                 stUrsaExp &symbolTable);

  static bool Ackermannize();
  static bool HasArrays();
  static UrsaExp GetAbstractLA(caExp &e);
  static UrsaExp GetAbstractASS(caExp &e);
  static UrsaExp AbstrAck(caExp &e, stUrsaExp &symbolTable, cStr &abs);
  static UrsaExp GetImplication(caExp &e1, caExp &e2, cStr &abs,
                                std::map<aExp, aExp> &ackselects,
                                stUrsaExp &symbolTable);
  static void PrepareNoAck(caExp &a, caExp &b, aExp &abs_a, aExp &abs_b,
                           aExp &abs_neg_b);
  static void Prepare(caExp &a, caExp &b, aExp &abs_a, aExp &abs_b,
                      aExp &abs_neg_b, std::set<aExp> &ls, std::set<aExp> &rs);
  void PrepareAckIncremental(caExp &a, caExp &b, aExp &abs_a, aExp &abs_b,
                             aExp &abs_neg_b, std::set<aExp> &ls,
                             std::set<aExp> &rs);
  static void PrepareAck(caExp &a, caExp &b, aExp &abs_a, aExp &abs_b,
                         aExp &abs_neg_b, std::set<aExp> &ls,
                         std::set<aExp> &rs);

private:
  thread_local static ExpFactory _Factory;
  thread_local static bool _BV;
  stUrsaExp _SymbolTable;

  std::set<aExp> _Lefts;
  std::set<aExp> _Rights;
  std::map<aExp, aExp> _Acks;
  thread_local static std::map<std::string, long> _Addresses;
  thread_local static std::map<aExp, UrsaExp> _AbstractLA;
  aExp _ExpInSolver;
  aExp _ExpToAddIntoSolver;
};

} //end of namespace

#endif
