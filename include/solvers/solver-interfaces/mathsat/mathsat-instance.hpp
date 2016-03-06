#if !defined(__MATHSAT_INSTANCE__)
#define __MATHSAT_INSTANCE__

extern "C" {
#include <mathsat.h>
}

namespace UrsaMajor {
  class MathSATInstance {
  public:
    ~MathSATInstance() {
      msat_destroy_env(_env);
    }


    void push() {
      if (!_blocking_clause_set) {
	msat_push_backtrack_point(_env);
      }
    }
    
    void pop() {
      msat_pop_backtrack_point(_env);
      _blocking_clause_set = false;
    }

    void reset();

    static MathSATInstance& instance() {
      static MathSATInstance _instance;
      return _instance;
    }

    msat_env getSolver() {
      return _env;
    }

    bool nextModel(msat_term expr) {
      if (!_blocking_clause_set) {
	msat_assert_formula(_env, expr);
      } else {
	msat_assert_formula(_env, _blocking_clause);
      }
      _blocking_clause_set = true;
      _blocking_clause = msat_make_false(_env);
      return msat_solve(_env) == MSAT_SAT;
    }

    bool addTempConstraint(msat_term expr);
    bool addConstraint(msat_term expr);

    std::string getAssignment(msat_term expr) {
      std::cout <<  msat_term_repr(expr) << std::endl;
      msat_term val = msat_get_model_value(_env, expr);
      char* r = msat_term_repr(val);
      if (r) {
	block(expr, val);
	std::stringstream s;
	s << r;
	free(r);
	return s.str();
      } else {
	return "error";
      }
    }
    std::map<std::string, msat_decl> _uf_registry;

  private:
    void block(msat_term expr, msat_term val) {
      msat_term diseq = msat_make_not(_env, msat_make_equal(_env, expr, val));
      _blocking_clause = msat_make_or(_env, _blocking_clause, diseq);
    }

    MathSATInstance() {
      _env = msat_create_env();
     msat_add_theory(_env, MSAT_UF);
     msat_add_theory(_env, MSAT_LIA);
     msat_add_theory(_env, MSAT_WORD);
     msat_set_theory_combination(_env, MSAT_COMB_ACK);
     _blocking_clause_set = false;
    }

    msat_env _env;
    msat_term _blocking_clause;
    bool _blocking_clause_set;
    static unsigned _pushed;

  };
} // namespace UrsaMajor

#endif
