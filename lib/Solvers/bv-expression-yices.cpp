#include "config.h"
#ifdef YICES

#include "solvers/solver-interfaces/yices/bv-expression-yices.hpp"

namespace UrsaMajor {

    ExpressionImp* BVExpressionImpYices::uninterpretedFunction(const Function& fun, const std::vector<const ExpressionImp*>& args) {
      size_t n = args.size();
//static std::map<std::string, yices_expr> _uf_registry;

std::map<std::string, yices_expr>& _uf_registry = YicesInstance::instance()._uf_registry;

      SOLVER_EXPR_TYPE f;
      if (_uf_registry.find(fun.getName()) == _uf_registry.end()) {
        yices_type* domain_types = new yices_type[n];
        for (size_t k = 0; k < n; k++)
          domain_types[k] = translateType(fun.getArgumentType(k));
        yices_type result_type = translateType(fun.getType());
        yices_type fty = yices_mk_function_type(getSolver(), domain_types, n, result_type);
        delete[] domain_types;
        
        yices_var_decl fdecl = yices_mk_var_decl(getSolver(), const_cast<char*>(fun.getName().c_str()), fty);
        f = yices_mk_var_from_decl(getSolver(), fdecl);
        _uf_registry[fun.getName()] = f;
      } else {
        f = _uf_registry[fun.getName()];
      }

      SOLVER_EXPR_TYPE* exps = new SOLVER_EXPR_TYPE[n];

      std::vector<const ExpressionImp*>::const_iterator i;
      int k;
      for (i = args.begin(), k = 0; i != args.end(); i++, k++) {
        const ExpressionImpGroundInteger* u = 
          llvm::dyn_cast<ExpressionImpGroundInteger>(*i);
        const ExpressionImpGroundBoolean* b = 
          llvm::dyn_cast<ExpressionImpGroundBoolean>(*i);
        const BVExpressionImpYices* y = 
          llvm::dyn_cast<BVExpressionImpYices>(*i);
        if (u != 0) {
          exps[k] = solverUnsignedExprFromGround(u);
        } else if (b != 0) {
          exps[k] = solverBooleanExprFromGround(b);
        } else {
          assert(y != 0);
          exps[k] = y->_expr;
        }
      }

      SOLVER_EXPR_TYPE exp = yices_mk_app(getSolver(), f, exps, n);
      delete[] exps;
      return new BVExpressionImpYices(exp, fun.getType().getWidth(), YICES_BITVECTOR);
    }


//FIXME ovo je krpljeno u odnosu na filipovo
    BVExpressionImpYices::SOLVER_EXPR_TYPE BVExpressionImpYices::solverUnsignedExprFromGround(const ExpressionImpGroundInteger* const e) const {
      unsigned value = e->getUnsignedValue();
      return yices_mk_bv_constant(getSolver(), e->GetWidth(), value);
    }



    yices_type BVExpressionImpYices::translateType(Type t) {
      yices_type unsigned_type = yices_mk_type(getSolver(), const_cast<char*>("int"));
      yices_type bool_type = yices_mk_type(getSolver(), const_cast<char*>("bool"));
      yices_type  bv_type = yices_mk_bitvector_type(getSolver(), t.getWidth());
      switch (t.getType()) {
      case UNSIGNED:
        return unsigned_type;
      case BOOLEAN:
        return bool_type;
      case BITVECTOR:
//std::cout << "t.getWidth()= "<< t.getWidth() <<std::endl;
        return bv_type;
      default:
        throw "Unsupported type for uninterpreted function";
      }
    }
} // namespace UrsaMajor

#endif
