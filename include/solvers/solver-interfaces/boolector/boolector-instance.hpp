#if !defined __BOOLECTOR_INSTANCE__
#define __BOOLECTOR_INSTANCE__

#include <string>
#include <vector>
#include <stdio.h>

extern "C" {
#include "boolector.h"
}

namespace UrsaMajor {
#if defined(BOOLECTOR)
typedef BoolectorNode *SOLVER_EXPR_TYPE;
#endif
#if defined(BOOLECTOR_OLD)
typedef BtorExp *SOLVER_EXPR_TYPE;
#endif

class BoolectorInstance {
public:
  static BoolectorInstance &instance() {
    static BoolectorInstance _instance;
    return _instance;
  }

  Btor *getSolver() const { return _btor; }

  bool addConstraint(SOLVER_EXPR_TYPE expr) {
    // boolector_dump_smt(_btor, stdout, expr);
    _solver_called++;
    boolector_assert(_btor, expr);
    return true;
  }

  bool addTempConstraint(SOLVER_EXPR_TYPE expr) {
    _solver_called++;
    boolector_assume(_btor, expr);
    return boolector_sat(_btor) == BOOLECTOR_SAT;
  }

  bool nextModel(SOLVER_EXPR_TYPE expr);
  std::string getAssignment(SOLVER_EXPR_TYPE expr);
  void block(SOLVER_EXPR_TYPE expr, unsigned val);
  void ExpToRelease(SOLVER_EXPR_TYPE expr) {
    _exprs_to_release.push_back(expr);
  }

  void reset();

private:

  void BoolectorRelease();
  void BoolectorDeinit();
  void BoolectorInit();

  BoolectorInstance() { BoolectorInit(); }
  ~BoolectorInstance() { BoolectorDeinit(); }

  unsigned _solver_called;
  Btor *_btor;
  SOLVER_EXPR_TYPE _blocking_clause;
  std::vector<SOLVER_EXPR_TYPE> _exprs_to_release;

};
} // namespace UrsaMajor

#endif
