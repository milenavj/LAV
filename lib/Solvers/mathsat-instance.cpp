#include "config.h"
#ifdef MATHSAT

#include "solvers/solver-interfaces/mathsat/bv-expression-mathsat.hpp"

namespace UrsaMajor {
unsigned MathSATInstance::_pushed = 0;

    bool MathSATInstance::addConstraint(msat_term expr) {
       msat_assert_formula(_env, expr);
      return true;
    }



    bool MathSATInstance::addTempConstraint(msat_term expr) {
       msat_push_backtrack_point(_env);
       msat_assert_formula(_env, expr);
      bool result = (msat_solve(_env) == MSAT_SAT);
      msat_pop_backtrack_point(_env);
      return result;
    }

    void MathSATInstance::reset() {
//      std::cout << "MathSATInstance::reset()..." << std::endl;
      msat_destroy_env(_env);
      _env = msat_create_env();
      msat_add_theory(_env, MSAT_UF);
      msat_add_theory(_env, MSAT_LIA);
      msat_add_theory(_env, MSAT_WORD);
      msat_set_theory_combination(_env, MSAT_COMB_ACK);
      _blocking_clause_set = false;
     _uf_registry.clear();
    }

} // namespace UrsaMajor
#endif