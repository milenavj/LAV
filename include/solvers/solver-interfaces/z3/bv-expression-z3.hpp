#if !defined __BV_EXPRESSION_Z3__
#define __BV_EXPRESSION_Z3__

#include <cassert>

#include "../expression.hpp"
#include "z3-instance.hpp"

namespace UrsaMajor {
class BVExpressionImpZ3 : public ExpressionImpSymbolic {
public:
  typedef Z3_ast SOLVER_EXPR_TYPE;
  typedef Z3_context SOLVER_TYPE;
  enum Z3Type {
    Z3_BOOLEAN,
    Z3_BITVECTOR,
    Z3_ARRAY
  };

  explicit BVExpressionImpZ3()
      : ExpressionImpSymbolic(EK_ExpressionImpZ3), _expr(0) {}

  explicit BVExpressionImpZ3(SOLVER_EXPR_TYPE expr, size_t width, Z3Type type)
      : ExpressionImpSymbolic(EK_ExpressionImpZ3), _expr(expr), _width(width)
        //        _type(type)
        {}

  static bool classof(const ExpressionImp *S) {
    return S->getKind() == EK_ExpressionImpZ3;
  }

  bool checkSat() {
    //      std::cout << "Z3::searching..." << std::endl;
    //    Z3_API Z3_set_ast_print_mode(getSolver(), Z3_PRINT_SMTLIB_COMPLIANT);
    //    std::cout << Z3_API Z3_ast_to_string(getSolver(), _expr);

    return Z3Instance::instance().nextModel(_expr);
  }

  bool addTempConstraint() {
    //    std::cout << "addTempConstraint Z3::searching..." << std::endl;
    bool b = Z3Instance::instance().addTempConstraint(_expr);
    //    std::cout << "addTempConstraint Z3::searching...Finished" <<
    // std::endl;
    return b;
  }

  bool addConstraint() {
    //      std::cout << "addConstraint Z3::searching..." << std::endl;
    bool b = Z3Instance::instance().addConstraint(_expr);
    //      std::cout << "addConstraint Z3::searching...Finished" << std::endl;
    return b;
  }

  //  ovo se nikada ne poziva?
  void reset() {
    std::cout << "Z3 Reseting context..." << std::endl;
    Z3Instance::instance().reset();
  }

  std::string getAssignment() const {
    //      if (_type == Z3_BITVECTOR) {
    return Z3Instance::instance().getAssignment(_expr, _width);
    //      } else if (_type == Z3_BOOLEAN) {
    //	return Z3Instance::instance().getBooleanAssignment(_expr);
    //      }
    //      return "error";
  }

  Z3_sort translateType(Type t) {
    thread_local static Z3_sort unsigned_type = Z3_mk_int_sort(getSolver());
    thread_local static Z3_sort bool_type = Z3_mk_bool_sort(getSolver());
    Z3_sort bv_type = Z3_mk_bv_sort(getSolver(), t.getWidth());
    switch (t.getType()) {
    case UNSIGNED:
      return unsigned_type;
    case BOOLEAN: { return bool_type; }
    case BITVECTOR: { return bv_type; }
    default:
      throw "Unsupported type for uninterpreted function";
    }
  }

  virtual ExpressionImp *array(const std::string &name, size_t domain,
                               size_t range) {
    Z3_sort z3domain = Z3_mk_bv_sort(getSolver(), domain);
    Z3_sort z3range = Z3_mk_bv_sort(getSolver(), range);
    Z3_sort a = Z3_mk_array_sort(getSolver(), z3domain, z3range);
    Z3_symbol s = Z3_mk_string_symbol(getSolver(), name.c_str());
    SOLVER_EXPR_TYPE exp = Z3_mk_const(getSolver(), s, a);
    //FIXME ovo width nije jasno sta je
    return new BVExpressionImpZ3(exp, range, Z3_ARRAY);
  }

  virtual ExpressionImp *select(const ExpressionImp *const array,
                                const ExpressionImp *const index) const {
    const BVExpressionImpZ3 *const sarray =
        llvm::dyn_cast<BVExpressionImpZ3>(array);

    SOLVER_EXPR_TYPE exp =
        Z3_mk_select(getSolver(), sarray->_expr, solverExprFromUnknown(index));
    //FIXME ovo width nije jasno sta je
    return new BVExpressionImpZ3(exp, sarray->_width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *store(const ExpressionImp *const array,
                               const ExpressionImp *const index,
                               const ExpressionImp *const value) const {
    const BVExpressionImpZ3 *sarray = llvm::dyn_cast<BVExpressionImpZ3>(array);
    SOLVER_EXPR_TYPE eindex = solverExprFromUnknown(index);
    SOLVER_EXPR_TYPE evalue = solverExprFromUnknown(value);
    SOLVER_EXPR_TYPE exp =
        Z3_mk_store(getSolver(), sarray->_expr, eindex, evalue);
    //FIXME ovo width nije jasno sta je
    return new BVExpressionImpZ3(exp, sarray->_width, Z3_BITVECTOR);

  }

  SOLVER_EXPR_TYPE solverExprFromUnknown(const ExpressionImp *const e) const {
    const ExpressionImpGroundInteger *const u =
        llvm::dyn_cast<ExpressionImpGroundInteger>(e);
    if (u != 0) {
      SOLVER_EXPR_TYPE set = solverUnsignedExprFromGround(u);
      return set;
    }

    const ExpressionImpGroundBoolean *const b =
        llvm::dyn_cast<ExpressionImpGroundBoolean>(e);
    if (b != 0)
      return solverBooleanExprFromGround(b);

    const BVExpressionImpZ3 *const s = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(s != 0);
    return s->_expr;
  }

  thread_local static std::map<std::string, Z3_func_decl> _uf_registry;

  virtual ExpressionImp *
  uninterpretedFunction(const Function &fun,
                        const std::vector<const ExpressionImp *> &args) {
    size_t n = args.size();

    Z3_func_decl f;
    if (_uf_registry.find(fun.getName()) == _uf_registry.end()) {
      Z3_sort *domain_types = new Z3_sort[n + 1];
      for (size_t k = 0; k < n; k++)
        domain_types[k] = translateType(fun.getArgumentType(k));
      Z3_sort result_type = translateType(fun.getType());
      f = Z3_mk_func_decl(
          getSolver(), Z3_mk_string_symbol(getSolver(), fun.getName().c_str()),
          n, domain_types, result_type);
      _uf_registry[fun.getName()] = f;
      delete[] domain_types;
    } else {
      f = _uf_registry[fun.getName()];
    }

    SOLVER_EXPR_TYPE *exps = new SOLVER_EXPR_TYPE[n + 1];

    std::vector<const ExpressionImp *>::const_iterator i;
    int k;
    for (i = args.begin(), k = 0; i != args.end(); i++, k++) {
      exps[k] = solverExprFromUnknown(*i);
      /*        const ExpressionImpGroundInteger* u =
                llvm::dyn_cast<const ExpressionImpGroundInteger*>(*i);
              const ExpressionImpGroundBoolean* b =
                llvm::dyn_cast<const ExpressionImpGroundBoolean*>(*i);
              const BVExpressionImpZ3* y =
                llvm::dyn_cast<const BVExpressionImpZ3*>(*i);
              if (u != 0) {
                exps[k] = solverUnsignedExprFromGround(u);
              } else if (b != 0) {
                exps[k] = solverBooleanExprFromGround(b);
              } else {
                assert(y != 0);
                exps[k] = y->_expr;
              }*/
    }

    SOLVER_EXPR_TYPE exp = Z3_mk_app(getSolver(), f, n, exps);
    delete[] exps;
    return new BVExpressionImpZ3(exp, fun.getType().getWidth(), Z3_BITVECTOR);
  }

  virtual ExpressionImp *unsignedSymbolic(const std::string &name,
                                          size_t width) const {
    Z3_sort sort = Z3_mk_bv_sort(getSolver(), width);
    Z3_symbol s = Z3_mk_string_symbol(getSolver(), name.c_str());
    SOLVER_EXPR_TYPE exp = Z3_mk_const(getSolver(), s, sort);
    return new BVExpressionImpZ3(exp, width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *signedSymbolic(const std::string &name,
                                        size_t width) const {
    Z3_sort sort = Z3_mk_bv_sort(getSolver(), width);
    Z3_symbol s = Z3_mk_string_symbol(getSolver(), name.c_str());
    SOLVER_EXPR_TYPE exp = Z3_mk_const(getSolver(), s, sort);
    return new BVExpressionImpZ3(exp, width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *integerGround(unsigned x, size_t width) const {
    Z3_sort sort = Z3_mk_bv_sort(getSolver(), width);
    SOLVER_EXPR_TYPE exp =
        Z3_mk_numeral(getSolver(), (toString(x)).c_str(), sort);
    return new BVExpressionImpZ3(exp, width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *integerGround(int x, size_t width) const {
    Z3_sort sort = Z3_mk_bv_sort(getSolver(), width);
    SOLVER_EXPR_TYPE exp =
        Z3_mk_numeral(getSolver(), (toString(x)).c_str(), sort);
    return new BVExpressionImpZ3(exp, width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *integerGround(unsigned long x, size_t width) const {
    Z3_sort sort = Z3_mk_bv_sort(getSolver(), width);
    SOLVER_EXPR_TYPE exp =
        Z3_mk_numeral(getSolver(), (toString(x)).c_str(), sort);
    return new BVExpressionImpZ3(exp, width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *integerGround(signed long x, size_t width) const {
    Z3_sort sort = Z3_mk_bv_sort(getSolver(), width);
    SOLVER_EXPR_TYPE exp =
        Z3_mk_numeral(getSolver(), (toString(x)).c_str(), sort);
    return new BVExpressionImpZ3(exp, width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *integerGround(unsigned short x, size_t width) const {
    Z3_sort sort = Z3_mk_bv_sort(getSolver(), width);
    SOLVER_EXPR_TYPE exp =
        Z3_mk_numeral(getSolver(), (toString(x)).c_str(), sort);
    return new BVExpressionImpZ3(exp, width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *integerGround(signed short x, size_t width) const {
    Z3_sort sort = Z3_mk_bv_sort(getSolver(), width);
    SOLVER_EXPR_TYPE exp =
        Z3_mk_numeral(getSolver(), (toString(x)).c_str(), sort);
    return new BVExpressionImpZ3(exp, width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *booleanSymbolic(const std::string &name) const {
    Z3_sort ty = Z3_mk_bool_sort(getSolver());
    Z3_symbol s = Z3_mk_string_symbol(getSolver(), name.c_str());
    SOLVER_EXPR_TYPE exp = Z3_mk_const(getSolver(), s, ty);
    return new BVExpressionImpZ3(exp, 1, Z3_BOOLEAN);
  }

  virtual ExpressionImp *booleanGround(bool x) const {
    SOLVER_EXPR_TYPE exp =
        x ? Z3_mk_true(getSolver()) : Z3_mk_false(getSolver());
    return new BVExpressionImpZ3(exp, 1, Z3_BOOLEAN);
  }

  virtual ExpressionImp *
  addGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_bvadd(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  addSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvadd(getSolver(), this->_expr, bexpr->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  subtractGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_bvsub(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  subtractFromGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_bvsub(getSolver(), solverUnsignedExprFromGround(e), this->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  subtractSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvsub(getSolver(), this->_expr, bexpr->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  subtractFromSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvsub(getSolver(), bexpr->_expr, this->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  multGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_bvmul(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  multSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvmul(getSolver(), this->_expr, bexpr->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  udivGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_bvudiv(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  udivFromGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_bvudiv(getSolver(), solverUnsignedExprFromGround(e), this->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  udivSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvudiv(getSolver(), this->_expr, bexpr->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  udivFromSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvudiv(getSolver(), bexpr->_expr, this->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  sdivGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_bvsdiv(getSolver(), this->_expr, solverSignedExprFromGround(e));
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  sdivFromGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_bvsdiv(getSolver(), solverSignedExprFromGround(e), this->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  sdivSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvsdiv(getSolver(), this->_expr, bexpr->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  sdivFromSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvsdiv(getSolver(), bexpr->_expr, this->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  uremGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_bvurem(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  uremFromGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_bvurem(getSolver(), solverUnsignedExprFromGround(e), this->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  uremSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvurem(getSolver(), this->_expr, bexpr->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  uremFromSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvurem(getSolver(), bexpr->_expr, this->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  sremGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_bvsrem(getSolver(), this->_expr, solverSignedExprFromGround(e));
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  sremFromGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_bvsrem(getSolver(), solverSignedExprFromGround(e), this->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  sremSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvsrem(getSolver(), this->_expr, bexpr->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  sremFromSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvsrem(getSolver(), bexpr->_expr, this->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  shiftLGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_bvshl(getSolver(), solverUnsignedExprFromGround(e), this->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  shiftLForGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_bvshl(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  shiftLSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvshl(getSolver(), bexpr->_expr, this->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  shiftLForSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvshl(getSolver(), this->_expr, bexpr->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  aShiftRForGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_bvashr(getSolver(), this->_expr, solverSignedExprFromGround(e));
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  aShiftRGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_bvashr(getSolver(), solverSignedExprFromGround(e), this->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  aShiftRForSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvashr(getSolver(), this->_expr, bexpr->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  aShiftRSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvashr(getSolver(), bexpr->_expr, this->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  lShiftRGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_bvlshr(getSolver(), solverUnsignedExprFromGround(e), this->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  lShiftRForGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_bvlshr(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  lShiftRForSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvlshr(getSolver(), this->_expr, bexpr->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  lShiftRSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvlshr(getSolver(), bexpr->_expr, this->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  bitXorGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_bvxor(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  bitXorSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvxor(getSolver(), this->_expr, bexpr->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  bitAndGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_bvand(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  bitAndSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvand(getSolver(), this->_expr, bexpr->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  logicalAndGround(const ExpressionImpGroundBoolean *const e) const {
    SOLVER_EXPR_TYPE args[2];
    args[0] = this->_expr;
    args[1] = solverBooleanExprFromGround(e);
    SOLVER_EXPR_TYPE exp = Z3_mk_and(getSolver(), 2, args);
    return new BVExpressionImpZ3(exp, _width, Z3_BOOLEAN);
  }

  virtual ExpressionImp *
  logicalAndSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE args[2];
    args[0] = this->_expr;
    args[1] = bexpr->_expr;
    SOLVER_EXPR_TYPE exp = Z3_mk_and(getSolver(), 2, args);
    return new BVExpressionImpZ3(exp, _width, Z3_BOOLEAN);
  }

  virtual ExpressionImp *
  bitOrGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_bvor(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  bitOrSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvor(getSolver(), this->_expr, bexpr->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  logicalOrGround(const ExpressionImpGroundBoolean *const e) const {
    SOLVER_EXPR_TYPE args[2];
    args[0] = this->_expr;
    args[1] = solverBooleanExprFromGround(e);
    SOLVER_EXPR_TYPE exp = Z3_mk_or(getSolver(), 2, args);
    return new BVExpressionImpZ3(exp, _width, Z3_BOOLEAN);
  }

  virtual ExpressionImp *
  logicalOrSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE args[2];
    args[0] = this->_expr;
    args[1] = bexpr->_expr;
    SOLVER_EXPR_TYPE exp = Z3_mk_or(getSolver(), 2, args);
    return new BVExpressionImpZ3(exp, _width, Z3_BOOLEAN);
  }

  virtual ExpressionImp *bitNot() const {
    SOLVER_EXPR_TYPE exp = Z3_mk_bvnot(getSolver(), this->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *logicalNot() const {
    SOLVER_EXPR_TYPE exp = Z3_mk_not(getSolver(), this->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BOOLEAN);
  }

  virtual ExpressionImp *
  sltGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_bvslt(getSolver(), this->_expr, solverSignedExprFromGround(e));
    return new BVExpressionImpZ3(exp, 1, Z3_BOOLEAN);
  }

  virtual ExpressionImp *
  sltSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvslt(getSolver(), this->_expr, bexpr->_expr);
    return new BVExpressionImpZ3(exp, 1, Z3_BOOLEAN);
  }

  virtual ExpressionImp *
  sgtGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_bvsgt(getSolver(), this->_expr, solverSignedExprFromGround(e));
    return new BVExpressionImpZ3(exp, 1, Z3_BOOLEAN);
  }

  virtual ExpressionImp *
  sgtSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvsgt(getSolver(), this->_expr, bexpr->_expr);
    return new BVExpressionImpZ3(exp, 1, Z3_BOOLEAN);
  }

  virtual ExpressionImp *
  sleGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_bvsle(getSolver(), this->_expr, solverSignedExprFromGround(e));
    return new BVExpressionImpZ3(exp, 1, Z3_BOOLEAN);
  }

  virtual ExpressionImp *
  sleSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvsle(getSolver(), this->_expr, bexpr->_expr);
    return new BVExpressionImpZ3(exp, 1, Z3_BOOLEAN);
  }

  virtual ExpressionImp *
  sgeGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_bvsge(getSolver(), this->_expr, solverSignedExprFromGround(e));
    return new BVExpressionImpZ3(exp, 1, Z3_BOOLEAN);
  }

  virtual ExpressionImp *
  sgeSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvsge(getSolver(), this->_expr, bexpr->_expr);
    return new BVExpressionImpZ3(exp, 1, Z3_BOOLEAN);
  }

  virtual ExpressionImp *
  ultGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_bvult(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    return new BVExpressionImpZ3(exp, 1, Z3_BOOLEAN);
  }

  virtual ExpressionImp *
  ultSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvult(getSolver(), this->_expr, bexpr->_expr);
    return new BVExpressionImpZ3(exp, 1, Z3_BOOLEAN);
  }

  virtual ExpressionImp *
  ugtGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_bvugt(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    return new BVExpressionImpZ3(exp, 1, Z3_BOOLEAN);
  }

  virtual ExpressionImp *
  ugtSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvugt(getSolver(), this->_expr, bexpr->_expr);
    return new BVExpressionImpZ3(exp, 1, Z3_BOOLEAN);
  }

  virtual ExpressionImp *
  uleGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_bvule(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    return new BVExpressionImpZ3(exp, 1, Z3_BOOLEAN);
  }

  virtual ExpressionImp *
  uleSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvule(getSolver(), this->_expr, bexpr->_expr);
    return new BVExpressionImpZ3(exp, 1, Z3_BOOLEAN);
  }

  virtual ExpressionImp *
  ugeGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_bvuge(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    return new BVExpressionImpZ3(exp, 1, Z3_BOOLEAN);
  }

  virtual ExpressionImp *
  ugeSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvuge(getSolver(), this->_expr, bexpr->_expr);
    return new BVExpressionImpZ3(exp, 1, Z3_BOOLEAN);
  }

  //FIXME da li ovo moze da bude negativno width - _width
  virtual ExpressionImp *zextimp(size_t width) const {
    assert((width - _width) > 0);
    //if((width - _width)<=0) std::cout << "zext********************************
    //" << width - _width << " ********************************"<<
    //std::endl<<std::endl<<std::endl;
    SOLVER_EXPR_TYPE exp =
        Z3_mk_zero_ext(getSolver(), width - _width, this->_expr);
    return new BVExpressionImpZ3(exp, width, Z3_BITVECTOR);
  }

  //FIXME da li ovo moze da bude negativno width - _width
  virtual ExpressionImp *sextimp(size_t width) const {
    assert((width - _width) > 0);
    //if((width - _width)<=0) std::cout << "sext********************************
    //" << width - _width << " ********************************"<<
    //std::endl<<std::endl<<std::endl;
    SOLVER_EXPR_TYPE exp =
        Z3_mk_sign_ext(getSolver(), width - _width, this->_expr);
    return new BVExpressionImpZ3(exp, width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *extractimp(size_t low, size_t high) const {
    SOLVER_EXPR_TYPE exp = Z3_mk_extract(getSolver(), high, low, this->_expr);
    return new BVExpressionImpZ3(exp, high - low + 1, Z3_BITVECTOR);
  }

  virtual ExpressionImp *
  eqGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_eq(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    return new BVExpressionImpZ3(exp, 1, Z3_BOOLEAN);
  }

  virtual ExpressionImp *
  eqGround(const ExpressionImpGroundBoolean *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_eq(getSolver(), this->_expr, solverBooleanExprFromGround(e));
    return new BVExpressionImpZ3(exp, 1, Z3_BOOLEAN);
  }

  virtual ExpressionImp *
  eqSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_eq(getSolver(), this->_expr, bexpr->_expr);
    return new BVExpressionImpZ3(exp, 1, Z3_BOOLEAN);
  }

  virtual ExpressionImp *
  neqGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp = Z3_mk_not(
        getSolver(),
        Z3_mk_eq(getSolver(), this->_expr, solverUnsignedExprFromGround(e)));
    return new BVExpressionImpZ3(exp, 1, Z3_BOOLEAN);
  }

  virtual ExpressionImp *
  neqGround(const ExpressionImpGroundBoolean *const e) const {
    SOLVER_EXPR_TYPE exp = Z3_mk_not(
        getSolver(),
        Z3_mk_eq(getSolver(), solverBooleanExprFromGround(e), this->_expr));
    return new BVExpressionImpZ3(exp, 1, Z3_BOOLEAN);
  }

  virtual ExpressionImp *
  neqSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_not(
        getSolver(), Z3_mk_eq(getSolver(), this->_expr, bexpr->_expr));
    return new BVExpressionImpZ3(exp, 1, Z3_BOOLEAN);
  }

  virtual ExpressionImp *ite_(ExpressionImp *thenExpr,
                              ExpressionImp *elseExpr) const {
    BVExpressionImpZ3 *bthenExpr = llvm::dyn_cast<BVExpressionImpZ3>(thenExpr);
    BVExpressionImpZ3 *belseExpr = llvm::dyn_cast<BVExpressionImpZ3>(elseExpr);
    assert(bthenExpr != 0 && belseExpr != 0);
    SOLVER_EXPR_TYPE exp =
        Z3_mk_ite(getSolver(), this->_expr, bthenExpr->_expr, belseExpr->_expr);
    return new BVExpressionImpZ3(exp, bthenExpr->_width, Z3_BITVECTOR);
  }

  virtual void print(std::ostream &ostr) const {
    ostr << Z3_ast_to_string(getSolver(), _expr);
  }

private:
  static SOLVER_TYPE getSolver() { return Z3Instance::instance().getSolver(); }

  SOLVER_EXPR_TYPE solverUnsignedExprFromGround(
      const ExpressionImpGroundInteger *const e) const {
    unsigned long value = e->getUnsignedValue();
    //FIXME reci filipu, ovde je bilo _width a ja sam promenila u e->getwidth -
    //i gore sam promenila tip
    //i u klasu sam ubacila getwidth jer se _width nije postavljalo u ovom
    //slucaju i pucalo e
    //ispravjljen je i neq
    Z3_sort sort = Z3_mk_bv_sort(getSolver(), e->GetWidth());
    SOLVER_EXPR_TYPE exp =
        Z3_mk_numeral(getSolver(), toString(value).c_str(), sort);
    return exp;
  }
  //FIXME kod mathsata i ovaj je morao da se ispravi!!!!
  SOLVER_EXPR_TYPE
  solverSignedExprFromGround(const ExpressionImpGround *const e) const {
    unsigned long value = e->getUnsignedValue();
    Z3_sort sort = Z3_mk_bv_sort(getSolver(), _width);
    SOLVER_EXPR_TYPE exp =
        Z3_mk_numeral(getSolver(), toString((signed long) value).c_str(), sort);
    return exp;
  }

  SOLVER_EXPR_TYPE
  solverBooleanExprFromGround(const ExpressionImpGround *const e) const {
    unsigned value = e->getBooleanValue();
    return value ? Z3_mk_true(getSolver()) : Z3_mk_false(getSolver());
  }

  SOLVER_EXPR_TYPE _expr;
  size_t _width;
  //  Z3Type _type;
};

class BVExpressionFactoryZ3 : public ExpressionFactory {
public:
  BVExpressionFactoryZ3() : ExpressionFactory(new BVExpressionImpZ3()) {}
};

} // namespace UrsaMajor

#endif
