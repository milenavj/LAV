#include "config.h"
#if defined(BOOLECTOR) || defined(BOOLECTOR_OLD)

#include "solvers/solver-interfaces/boolector/bv-expression-boolector.hpp"

namespace UrsaMajor {

bool BoolectorInstance::nextModel(SOLVER_EXPR_TYPE expr) {
  if (_blocking_clause == 0) {
    boolector_assert(_btor, expr);
  } else {
    boolector_assert(_btor, _blocking_clause);
  }

  _blocking_clause = boolector_false(_btor);
  _exprs_to_release.push_back(_blocking_clause);
  // FILE* f = fopen("smt", "w");
  // boolector_dump_smt(_btor, f, expr);
  // fclose(f);
  return boolector_sat(_btor) == BOOLECTOR_SAT;
}

std::string BoolectorInstance::getAssignment(SOLVER_EXPR_TYPE expr) {
#if defined(BOOLECTOR_OLD)
  char *s = boolector_bv_assignment(_btor, expr);
  std::string ss(s);
  free(s);
#endif
#if defined(BOOLECTOR)
  const char *s = boolector_bv_assignment(_btor, expr);
  std::string ss(s);
  boolector_free_bv_assignment(_btor, s);
#endif
  return ss;
}

void BoolectorInstance::BoolectorRelease() {
  std::vector<SOLVER_EXPR_TYPE>::iterator i;
  for (i = _exprs_to_release.begin(); i != _exprs_to_release.end(); i++)
    boolector_release(_btor, *i);
  _exprs_to_release.clear();
}

void BoolectorInstance::BoolectorDeinit() {
  BoolectorRelease();
  if (_solver_called) {
    assert(boolector_get_refs(_btor) == 0);
    boolector_delete(_btor);
  }
  _blocking_clause = 0;
}

void BoolectorInstance::BoolectorInit() {
  _btor = boolector_new();
#if defined(BOOLECTOR_OLD)
  boolector_enable_model_gen(_btor);
  boolector_enable_inc_usage(_btor);
#endif
#if defined(BOOLECTOR)
  BVExpressionImpBoolector::_uf_registry.clear();
  boolector_set_opt(_btor, "model_gen", 1);
  boolector_set_opt(_btor, "incremental", 1);
#endif
  _blocking_clause = 0;
  _solver_called = 0;
}

void BoolectorInstance::block(SOLVER_EXPR_TYPE expr, unsigned val) {
  SOLVER_EXPR_TYPE btor_val =
      boolector_unsigned_int(_btor, val, boolector_get_width(_btor, expr));

  _exprs_to_release.push_back(btor_val);

  SOLVER_EXPR_TYPE diseq = boolector_ne(_btor, expr, btor_val);
  _exprs_to_release.push_back(diseq);

  _blocking_clause = boolector_or(_btor, _blocking_clause, diseq);
  _exprs_to_release.push_back(_blocking_clause);
}

void BoolectorInstance::reset() {

  BoolectorRelease();
  if (_solver_called) {
    boolector_delete(_btor);
    BoolectorInit();
  }
  _blocking_clause = 0;
}

} // namespace UrsaMajor

#endif
