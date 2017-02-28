#include "config.h"
#ifdef MATHSAT

#include "solvers/solver-interfaces/mathsat/la-expression-mathsat.hpp"

namespace UrsaMajor {

static msat_type translateType(Type t) {
  static msat_type unsigned_type = MSAT_INT;
  static msat_type bool_type = MSAT_BOOL;
  switch (t.getType()) {
  case UNSIGNED:
    return unsigned_type;
  case BOOLEAN:
    return bool_type;
  default:
    throw "Unsupported type for uninterpreted function";
  }
}

ExpressionImp *LAExpressionImpMathSAT::uninterpretedFunction(
    const Function &fun, const std::vector<const ExpressionImp *> &args) {
  size_t n = args.size();

  //      static std::map<std::string, msat_decl> _uf_registry;
  std::map<std::string, msat_decl> &_uf_registry =
      MathSATInstance::instance()._uf_registry;
  msat_decl f;
  if (_uf_registry.find(fun.getName()) == _uf_registry.end()) {
    msat_type *domain_types = new msat_type[n + 1];
    for (size_t k = 0; k < n; k++)
      domain_types[k] = translateType(fun.getArgumentType(k));
    msat_type result_type = translateType(fun.getType());
    f = msat_declare_uif(getSolver(), fun.getName().c_str(), result_type, n,
                         domain_types);
    _uf_registry[fun.getName()] = f;
    delete[] domain_types;
  } else {
    f = _uf_registry[fun.getName()];
  }

  SOLVER_EXPR_TYPE *exps = new SOLVER_EXPR_TYPE[n];

  std::vector<const ExpressionImp *>::const_iterator i;
  int k;
  for (i = args.begin(), k = 0; i != args.end(); i++, k++) {
    const ExpressionImpGroundInteger *u =
        llvm::dyn_cast<ExpressionImpGroundInteger>(*i);
    const ExpressionImpGroundBoolean *b =
        llvm::dyn_cast<ExpressionImpGroundBoolean>(*i);
    const LAExpressionImpMathSAT *y =
        llvm::dyn_cast<LAExpressionImpMathSAT>(*i);
    if (u != 0) {
      //FIXME moze i signed
      exps[k] = solverSignedExprFromGround(u);
    } else if (b != 0) {
      exps[k] = solverBooleanExprFromGround(b);
    } else {
      assert(y != 0);
      exps[k] = y->_expr;
    }
  }

  SOLVER_EXPR_TYPE exp = msat_make_uif(getSolver(), f, exps);
  delete[] exps;
  return new LAExpressionImpMathSAT(exp);
}

} // namespace UrsaMajor
#endif
