#include "config.h"
#ifdef BOOLECTOR

#include "solvers/solver-interfaces/boolector/bv-expression-boolector.hpp"

namespace UrsaMajor {

    bool BoolectorInstance::nextModel(BtorExp* expr) {
std::cout << "nextModel " <<std::endl;
      if (_blocking_clause == 0) {
        boolector_assert(_btor, expr);
      } else {
        boolector_assert(_btor, _blocking_clause);
      }

      _blocking_clause = boolector_false(_btor);
      _exprs_to_release.push_back(_blocking_clause);

//      FILE* f = fopen("smt", "w");
//      boolector_dump_smt(_btor, f, expr);
//      fclose(f);
      return boolector_sat(_btor) == BOOLECTOR_SAT;
    }
      
/*    unsigned BoolectorInstance::getAssignment(BtorExp* expr)  {
      char* s = boolector_bv_assignment(_btor, expr);
      std::string ss(s);
      unsigned number = binarystring2unsigned(ss);
      free(s);
      return number;
    }
*/

    std::string BoolectorInstance::getAssignment(BtorExp* expr)  {
      char* s = boolector_bv_assignment(_btor, expr);
      std::string ss(s);
      free(s);
//      block(expr, binarystring2unsigned(ss));
      return ss;
    }
    
    void BoolectorInstance::block(BtorExp* expr, unsigned val) {
      BtorExp* btor_val = boolector_unsigned_int(_btor, val, boolector_get_width(_btor, expr));
//std::cout << "block " <<std::endl;
      _exprs_to_release.push_back(btor_val);

      BtorExp* diseq = boolector_ne(_btor, expr, btor_val);
      _exprs_to_release.push_back(diseq);

      _blocking_clause = boolector_or(_btor, _blocking_clause, diseq);
      _exprs_to_release.push_back(_blocking_clause);
    }



} // namespace UrsaMajor

#endif
