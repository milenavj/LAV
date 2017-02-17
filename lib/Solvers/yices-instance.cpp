#include "config.h"
#ifdef YICES

#include "solvers/solver-interfaces/yices/yices-instance.hpp"

namespace UrsaMajor {

unsigned YicesInstance::_pushed = 0;

bool YicesInstance::addConstraint(yices_expr expr) {

  yices_push(_ctx);
  yices_assert(_ctx, expr);
  //      Yices_lbool result = Yices_check(_ctx);
  _pushed++;
  //      return result == Yices_L_TRUE;
  return true;
}

bool YicesInstance::addTempConstraint(yices_expr expr) {
  yices_push(_ctx);
  yices_assert(_ctx, expr);
  lbool result = yices_check(_ctx);
  yices_pop(_ctx);
  return result == l_true;

}

void YicesInstance::del() {
  _uf_registry.clear();
  yices_del_context(_ctx);
  _ctx = yices_mk_context();
  _blocking_clause = 0;
  yices_enable_type_checker(false);
}

//FIXME mozda treba vise pop-ova
void YicesInstance::reset() {

  /* for( ; _pushed>0; _pushed--)
        yices_pop(_ctx);
  _blocking_clause = 0;
  _uf_registry.clear();*/
  _uf_registry.clear();
  yices_del_context(_ctx);
  _ctx = yices_mk_context();
  _blocking_clause = 0;
  _pushed = 0;
  yices_enable_type_checker(false);

}

} // namespace UrsaMajor

#endif
