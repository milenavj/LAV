#if !defined __BOOLECTOR_INSTANCE__
#define __BOOLECTOR_INSTANCE__

#include <string>
#include <vector>
#include <stdio.h>

extern "C" {
#include "boolector.h"
}

namespace UrsaMajor {
  class BoolectorInstance {
  public:
    static BoolectorInstance& instance() {
      static BoolectorInstance _instance;
      return _instance;
    }

    Btor* getSolver() const {
      return _btor;
    }

    bool addConstraint(BtorExp* expr) 
    {
 //       boolector_dump_smt(_btor, stdout, expr);

_solver_called++;
        boolector_assert(_btor, expr);
//      _exprs_to_release.push_back(expr);
 
        return true; 
//      return boolector_sat(_btor) == BOOLECTOR_SAT;

/*      if (_blocking_clause == 0) {
        boolector_assert(_btor, expr);
      } else {
        boolector_assert(_btor, _blocking_clause);
      }

      _blocking_clause = boolector_false(_btor);
      _exprs_to_release.push_back(_blocking_clause);

      return boolector_sat(_btor) == BOOLECTOR_SAT;
*/    }

    bool addTempConstraint(BtorExp* expr) 
    {
_solver_called++;
      boolector_assume(_btor, expr);
//      _exprs_to_release.push_back(expr);
      return boolector_sat(_btor) == BOOLECTOR_SAT;
    }


    bool nextModel(BtorExp* expr) ;
    std::string getAssignment(BtorExp* expr) ;
    void block(BtorExp* expr, unsigned val) ;
    void ExpToRelease(BtorExp* expr) {_exprs_to_release.push_back(expr);}

    void reset() {
      // FIXME
      std::vector<BtorExp*>::iterator i;
      for (i = _exprs_to_release.begin(); 
           i != _exprs_to_release.end(); 
           i++)
           {
        boolector_release(_btor, *i);
        }
      _exprs_to_release.clear();
      if(_solver_called)
      {
      boolector_delete(_btor);
      _btor = boolector_new();
      boolector_enable_model_gen(_btor);
      boolector_enable_inc_usage(_btor);
      _blocking_clause = 0;
      _solver_called = 0; 
     }
      _blocking_clause = 0;
}

  private:
    ~BoolectorInstance() {
      std::vector<BtorExp*>::iterator i;
      for (i = _exprs_to_release.begin(); 
	   i != _exprs_to_release.end(); 
	   i++)
	boolector_release(_btor, *i);
if(_solver_called)
{
   assert (boolector_get_refs (btor) == 0);
        boolector_delete(_btor);
_solver_called = 0;
}
    }

    BoolectorInstance() {
      _btor = boolector_new();
      boolector_enable_model_gen(_btor);
      boolector_enable_inc_usage(_btor);
      _blocking_clause = 0;
      _solver_called = 0;
    }

    unsigned binarystring2unsigned(const std::string& s) {
      unsigned val = 0;
      for (std::string::const_iterator i = s.begin(); i != s.end();  i++)
	val = 2*val + (*i != '0');
      return val;
    }
  
  unsigned _solver_called;
    Btor* _btor;
    BtorExp* _blocking_clause;
    std::vector<BtorExp*> _exprs_to_release;

  };
} // namespace UrsaMajor

#endif
