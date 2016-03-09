#if !defined __YICES_INSTANCE__
#define __YICES_INSTANCE__

#include <string>
#include <iostream>
#include <cstdio>
#include <cassert>
#include <map>

#include "solvers/solver-interfaces/auxiliary.hpp"

extern "C" {
#include <yices_c.h>
//#include "../../third-party/solvers/yices-1.0.27/include/yices_c.h"
}

namespace UrsaMajor {
  class YicesInstance {
  public:
    static YicesInstance& instance() {
      static YicesInstance _instance;
      return _instance;
    }

    ~YicesInstance() {
      yices_del_context(_ctx);
    }

    yices_context getSolver() {
      return _ctx;
    }

    bool nextModel(yices_expr expr) {
      if (_blocking_clause == 0) {
	yices_push(_ctx);
	yices_assert(_ctx, expr);
      } else {
	yices_assert(_ctx, _blocking_clause);
      }
      _blocking_clause = yices_mk_false(_ctx);
      return yices_check(_ctx) == l_true;
    }

    bool addConstraint(yices_expr expr) ;
    bool addTempConstraint(yices_expr expr) ;
    void reset() ;
    void del() ;

/*    void reset() {
      yices_pop(_ctx);
      _blocking_clause = 0;
*/
//FIXME ovo je brisanje a ne resetovanje
/*      yices_del_context(_ctx);
      _ctx = yices_mk_context();
      _blocking_clause = 0;
      yices_enable_type_checker(false);
*/ 
//    }

    void block(yices_expr expr, int bv[], size_t width) {
      yices_expr bvconst = 
	yices_mk_bv_constant_from_array(_ctx, width, bv);
      yices_expr diseq = yices_mk_diseq(_ctx, expr, bvconst);
      yices_expr args[] = {_blocking_clause, diseq};
      _blocking_clause = yices_mk_or(_ctx, args, 2);
    }

    std::string getUnsignedAssignment(yices_expr expr, size_t width) {
      yices_model m = yices_get_model(_ctx);
      int* bv = new int[width];
      if (!yices_get_bitvector_value(m, yices_get_var_decl(expr), width, bv))
	return "error";
      std::string result = bitarray2string(bv, width);
      block(expr, bv, width);
      delete[] bv;
      return result;
    }

    void block(yices_expr expr, lbool v) {
      switch(v) {
      case l_true: {
	yices_expr neg = yices_mk_not(_ctx, expr);
	yices_expr args[] = {_blocking_clause, neg};
	_blocking_clause = yices_mk_or(_ctx, args, 2);
	break;
      }
      case l_false: {
	yices_expr args[] = {_blocking_clause, expr};
	_blocking_clause = yices_mk_or(_ctx, args, 2);
	break;
      }
      default:
	break;
      }
    }

    std::string getBooleanAssignment(yices_expr expr) {
      yices_model m = yices_get_model(_ctx);
      lbool v = yices_get_value(m, yices_get_var_decl(expr));
      std::string result = lbool2string(v);
      block(expr, v);
      return result;
    }

    void block(yices_expr expr, long v) {
      yices_expr vconst = 
	yices_mk_num(_ctx, v);
      yices_expr diseq = yices_mk_diseq(_ctx, expr, vconst);
      yices_expr args[] = {_blocking_clause, diseq};
      _blocking_clause = yices_mk_or(_ctx, args, 2);
    }

    std::string getIntAssignment(yices_expr expr) {
      yices_model m = yices_get_model(_ctx);
      long v;
      if (yices_get_int_value(m, yices_get_var_decl(expr), &v)) {
	block(expr, v);
	return toString(v);
      } else {
	return "error";
      }
    }

    std::map<std::string, yices_expr> _uf_registry;

  private:
    YicesInstance() {
      _ctx = yices_mk_context();
      _blocking_clause = 0;
      yices_enable_type_checker(false);
    }

    static std::string bitarray2string(int bv[], size_t width) {
      std::string result = "";
      for (int i = width - 1; i >= 0; i--)
	result += bv[i] ? "1" : "0";
      return result;
    }

    static std::string lbool2string(lbool v) {
      switch(v) {
      case l_true: 
	return "true";
      case l_false:
	return "false";
      default:
	return "error";
      }
    }


    static unsigned _pushed;
    yices_context _ctx;
    yices_expr _blocking_clause;
  };

} // namespace UrsaMajor

#endif
