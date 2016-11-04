#include "config.h"

#ifdef Z3

#include "solvers/solver-interfaces/z3/z3-instance.hpp"

namespace UrsaMajor {
void exitf(const char *message);
Z3_context mk_context();

thread_local unsigned Z3Instance::_pushed = 0;

Z3Instance &Z3Instance::instance() {
  thread_local static Z3Instance _instance;
  return _instance;
}

Z3Instance::Z3Instance() {
  _ctx = mk_context();
  _blocking_clause = 0;
  _m = 0;
}

Z3Instance::~Z3Instance() {
  if (_m)
    Z3_del_model(_ctx, _m);
  Z3_del_context(_ctx);
}

bool Z3Instance::nextModel(Z3_ast expr) {
  if (_blocking_clause == 0) {
    Z3_push(_ctx);
    Z3_assert_cnstr(_ctx, expr);
  } else {
    Z3_assert_cnstr(_ctx, _blocking_clause);
  }
  _blocking_clause = Z3_mk_false(_ctx);
  if (_m)
    Z3_del_model(_ctx, _m);
  _m = 0;
  Z3_lbool result = Z3_check_and_get_model(_ctx, &_m);

  return result == Z3_L_TRUE;
}

bool Z3Instance::addConstraint(Z3_ast expr) {
  Z3_push(_ctx);
  Z3_assert_cnstr(_ctx, expr);
  // Z3_lbool result = Z3_check(_ctx);
  _pushed++;
  // return result == Z3_L_TRUE;
  return true;
}

void display_model(Z3_context c, FILE *out, Z3_model m);

bool Z3Instance::addTempConstraint(Z3_ast expr) {
  if (_m)
    Z3_del_model(_ctx, _m);
  _m = 0;

  Z3_push(_ctx);
  Z3_assert_cnstr(_ctx, expr);

  // Z3_lbool result = Z3_check(_ctx);
  Z3_lbool result = Z3_check_and_get_model(_ctx, &_m);

  // if(result == Z3_L_TRUE)
  //    printf("sat\n%s\n", Z3_model_to_string(_ctx, _m));

  Z3_pop(_ctx, 1);
  return result == Z3_L_TRUE;
}

//FIXME mozda treba vise pop-ova
void Z3Instance::reset() {
  for (; _pushed > 0; _pushed--)
    Z3_pop(_ctx, 1);
  _m = 0;
  _blocking_clause = 0;
}

/**
   \brief Display a symbol in the given output stream.
*/
std::string display_symbol(Z3_context c, Z3_symbol s) {
  char ss[100];
  switch (Z3_get_symbol_kind(c, s)) {
  case Z3_INT_SYMBOL:
    sprintf(ss, "%d", Z3_get_symbol_int(c, s));
    return std::string(ss);
    break;
  case Z3_STRING_SYMBOL:
    return Z3_get_symbol_string(c, s);
    break;
  default:
    return "unknown";
  }
}

/**
   \brief Custom ast pretty printer.

   This function demonstrates how to use the API to navigate terms.
*/
std::string display_ast(Z3_context c, Z3_ast v) {
  switch (Z3_get_ast_kind(c, v)) {
  case Z3_NUMERAL_AST: {
    /* Z3_sort t;
       fprintf(out, Z3_get_numeral_string(c, v));
       t = Z3_get_sort(c, v);
       fprintf(out, ":");
       display_sort(c, out, t); */
    return Z3_get_numeral_string(c, v);
    break;
  }
  case Z3_APP_AST: {
    unsigned i;
    Z3_app app = Z3_to_app(c, v);
    unsigned num_fields = Z3_get_app_num_args(c, app);
    Z3_func_decl d = Z3_get_app_decl(c, app);
    std::string s = Z3_func_decl_to_string(c, d);
    if (s.find("true") != std::string::npos)
      return "1";
    if (s.find("false") != std::string::npos)
      return "0";
    if (num_fields > 0) {
      s.push_back('[');
      for (i = 0; i < num_fields; i++) {
        if (i > 0) {
          s.append(", ");
        }
        std::string s1 = display_ast(c, Z3_get_app_arg(c, app, i));
        s.append(s1);
      }
      s.push_back(']');
    }
    return s;
  }
  case Z3_QUANTIFIER_AST: { return "quantifier"; }
  default:
    return "unknown";
  }
  return "#unknown";
}

std::string Z3Instance::getAssignment(Z3_ast expr, size_t width) {
  Z3_ast v = expr;
  Z3_eval(_ctx, _m, expr, &v);
  return display_ast(_ctx, v);
}

void exitf(const char *message) {
  fprintf(stderr, "BUG: %s.\n", message);
  exit(1);
}

void error_handler(Z3_context c, Z3_error_code e) {
  printf("Error code: %d\n", e);
  exitf("incorrect use of Z3");
}

Z3_context mk_context_custom(Z3_config cfg, Z3_error_handler err) {
  Z3_context ctx;
  Z3_set_param_value(cfg, "MODEL", "true");
  ctx = Z3_mk_context(cfg);
  Z3_set_error_handler(ctx, err);
  return ctx;
}

Z3_context mk_context() {
  Z3_config cfg;
  Z3_context ctx;
  cfg = Z3_mk_config();
  ctx = mk_context_custom(cfg, error_handler);
  Z3_del_config(cfg);
  return ctx;
}

} // namespace UrsaMajor

#endif
