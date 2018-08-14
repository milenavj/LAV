#if !defined __LA_EXPRESSION_Z3__
#define __LA_EXPRESSION_Z3__

#include <cassert>
#include <map>

#include "../expression.hpp"
#include "z3-instance.hpp"

namespace UrsaMajor {

class LAExpressionImpZ3 : public ExpressionImpSymbolic {
public:
  typedef Z3_ast SOLVER_EXPR_TYPE;
  typedef Z3_context SOLVER_TYPE;

  enum Z3Type {
    Z3_BOOLEAN,
    Z3_BITVECTOR,
    Z3_ARRAY
  };

  LAExpressionImpZ3() : ExpressionImpSymbolic(EK_ExpressionImpZ3), _expr(0) {}

  static bool classof(const ExpressionImp *S) {
    return S->getKind() == EK_ExpressionImpZ3;
  }

  explicit LAExpressionImpZ3(SOLVER_EXPR_TYPE expr)
      : ExpressionImpSymbolic(EK_ExpressionImpZ3), _expr(expr) {}

  ~LAExpressionImpZ3() {}

  bool checkSat() {
    std::cout << "Z3::searching..." << std::endl;
    return Z3Instance::instance().nextModel(_expr);
  }
  bool addTempConstraint() {
    return Z3Instance::instance().addTempConstraint(_expr);
  }

  bool addConstraint() {
      return Z3Instance::instance().addConstraint(_expr);
  }

  void reset() {
    std::cout << "Reseting context..." << std::endl;
    Z3Instance::instance().reset();
  }

  std::string getAssignment() const {
    // TODO: FIXME: getIntAssignment vs getBooleanAssignment
    //      return Z3Instance::instance().getIntAssignment(_expr);
    return "error";
  }

  LAExpressionImpZ3 *unsignedSymbolic(const std::string &name,
                                      size_t width) const {
    Z3_sort sort = Z3_mk_int_sort(getSolver());
    Z3_symbol s = Z3_mk_string_symbol(getSolver(), name.c_str());
    SOLVER_EXPR_TYPE exp = Z3_mk_const(getSolver(), s, sort);
    print(exp, sort);
    return new LAExpressionImpZ3(exp);
  }

  //FIXME ovo se u yices-u razlikuje - nat i int????
  LAExpressionImpZ3 *signedSymbolic(const std::string &name,
                                    size_t width) const {
    Z3_sort sort = Z3_mk_int_sort(getSolver());
    Z3_symbol s = Z3_mk_string_symbol(getSolver(), name.c_str());
    SOLVER_EXPR_TYPE exp = Z3_mk_const(getSolver(), s, sort);
    print(exp, sort);
    return new LAExpressionImpZ3(exp);
  }

  LAExpressionImpZ3 *booleanSymbolic(const std::string &name) const {
    Z3_sort ty = Z3_mk_bool_sort(getSolver());
    Z3_symbol s = Z3_mk_string_symbol(getSolver(), name.c_str());
    SOLVER_EXPR_TYPE exp = Z3_mk_const(getSolver(), s, ty);
    print(exp,Z3_mk_bool_sort(getSolver()));
    return new LAExpressionImpZ3(exp);
  }

  LAExpressionImpZ3 *integerGround(unsigned int x, size_t width) const {
    Z3_sort sort = Z3_mk_int_sort(getSolver());
    SOLVER_EXPR_TYPE exp = Z3_mk_unsigned_int(getSolver(), x, sort);
    return new LAExpressionImpZ3(exp);
  }

  LAExpressionImpZ3 *integerGround(signed int x, size_t width) const {
    Z3_sort sort = Z3_mk_int_sort(getSolver());
    SOLVER_EXPR_TYPE exp = Z3_mk_int(getSolver(), x, sort);
    return new LAExpressionImpZ3(exp);
  }

  LAExpressionImpZ3 *integerGround(unsigned long int x, size_t width) const {
    Z3_sort sort = Z3_mk_int_sort(getSolver());
    SOLVER_EXPR_TYPE exp = Z3_mk_unsigned_int(getSolver(), x, sort);
    return new LAExpressionImpZ3(exp);
  }

  LAExpressionImpZ3 *integerGround(signed long int x, size_t width) const {
    Z3_sort sort = Z3_mk_int_sort(getSolver());
    SOLVER_EXPR_TYPE exp = Z3_mk_int(getSolver(), x, sort);
    return new LAExpressionImpZ3(exp);
  }

  LAExpressionImpZ3 *integerGround(unsigned short int x, size_t width) const {
    Z3_sort sort = Z3_mk_int_sort(getSolver());
    SOLVER_EXPR_TYPE exp = Z3_mk_unsigned_int(getSolver(), x, sort);
    return new LAExpressionImpZ3(exp);
  }

  LAExpressionImpZ3 *integerGround(signed short int x, size_t width) const {
    Z3_sort sort = Z3_mk_int_sort(getSolver());
    SOLVER_EXPR_TYPE exp = Z3_mk_int(getSolver(), x, sort);
    return new LAExpressionImpZ3(exp);
  }

  LAExpressionImpZ3 *booleanGround(bool x) const {
    SOLVER_EXPR_TYPE exp =
        x ? Z3_mk_true(getSolver()) : Z3_mk_false(getSolver());
    return new LAExpressionImpZ3(exp);
  }

  Z3_sort translateType(Type t) {
    static Z3_sort unsigned_type = Z3_mk_int_sort(getSolver());
    static Z3_sort bool_type = Z3_mk_bool_sort(getSolver());
    switch (t.getType()) {
    case UNSIGNED:
      return unsigned_type;
    case BOOLEAN:
      return bool_type;
    default:
      throw "Unsupported type for uninterpreted function";
    }
  }

  virtual ExpressionImp *
  uninterpretedFunction(const Function &fun,
                        const std::vector<const ExpressionImp *> &args) {
    size_t n = args.size();

    //ako bi bilo static, onda prilikom resetovanja ovde ostane u registru i
    //onda ili pukne program je se na adresi z3_func_decl nalazi nesto bezveze
    //ili ne pukne jer tu bude neka sasvim deseta funkcija, ali svakako bude
    //neka greska
    //problem je sto ako nema resetovanja izmedju, sta onda???!?? isto i za
    //bitvektore
    //static
    std::map<std::string, Z3_func_decl> _uf_registry;
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

    SOLVER_EXPR_TYPE *exps = new SOLVER_EXPR_TYPE[n];

    std::vector<const ExpressionImp *>::const_iterator i;
    int k;
    for (i = args.begin(), k = 0; i != args.end(); i++, k++) {

      const ExpressionImpGroundInteger *u =
          llvm::dyn_cast<ExpressionImpGroundInteger>(*i);

      const ExpressionImpGroundBoolean *b =
          llvm::dyn_cast<ExpressionImpGroundBoolean>(*i);

      const LAExpressionImpZ3 *y = llvm::dyn_cast<LAExpressionImpZ3>(*i);

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

    SOLVER_EXPR_TYPE exp = Z3_mk_app(getSolver(), f, n, exps);

    print(f);

    delete[] exps;
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *
  addGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE args[2];
    args[0] = this->_expr;
    //FIXME un-signed
    args[1] = solverSignedExprFromGround(e);
    SOLVER_EXPR_TYPE exp = Z3_mk_add(getSolver(), 2, args);
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *
  addSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpZ3 *const bexpr = llvm::dyn_cast<LAExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE args[2];
    args[0] = this->_expr;
    args[1] = bexpr->_expr;
    SOLVER_EXPR_TYPE exp = Z3_mk_add(getSolver(), 2, args);
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *
  addOverflowGround(const ExpressionImpGroundInteger *const e) const {
      throw "Z3LA - addSymbolic";
  }

  virtual ExpressionImp *
  addOverflowSymbolic(const ExpressionImpSymbolic *const e) const {
  throw "Z3LA - addSymbolic";
  }

  virtual ExpressionImp *
  subtractGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE args[2];
    args[0] = this->_expr;
    //FIXME
    args[1] = solverSignedExprFromGround(e);
    SOLVER_EXPR_TYPE exp = Z3_mk_sub(getSolver(), 2, args);
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *
  subtractFromGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE args[2];
    //FIXME
    args[0] = solverSignedExprFromGround(e);
    args[1] = this->_expr;
    SOLVER_EXPR_TYPE exp = Z3_mk_sub(getSolver(), 2, args);
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *
  subtractSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpZ3 *const bexpr = llvm::dyn_cast<LAExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE args[2];
    args[0] = this->_expr;
    args[1] = bexpr->_expr;
    SOLVER_EXPR_TYPE exp = Z3_mk_sub(getSolver(), 2, args);
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *
  subtractFromSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpZ3 *const bexpr = llvm::dyn_cast<LAExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE args[2];
    args[0] = bexpr->_expr;
    args[1] = this->_expr;
    SOLVER_EXPR_TYPE exp = Z3_mk_sub(getSolver(), 2, args);
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *
  multGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE args[2];
    args[0] = this->_expr;
    //FIXME signed
    args[1] = solverSignedExprFromGround(e);
    SOLVER_EXPR_TYPE exp = Z3_mk_mul(getSolver(), 2, args);
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *
  multSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  udivGround(const ExpressionImpGroundInteger *const e) const {
    throw "z3::udivGround Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  udivFromGround(const ExpressionImpGroundInteger *const e) const {
    throw "z3::udivFromGround Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  udivSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "z3::udivSymbolic Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  udivFromSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "z3::udivFromSymbolic Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  sdivGround(const ExpressionImpGroundInteger *const e) const {
    throw "z3::sdivGround Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  sdivFromGround(const ExpressionImpGroundInteger *const e) const {
    throw "z3::sdivFromGround Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  sdivSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "z3::sdivSymbolic Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  sdivFromSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "z3::sdivFromSymbolic Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  uremGround(const ExpressionImpGroundInteger *const e) const {
    throw "z3::uremGround Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  uremFromGround(const ExpressionImpGroundInteger *const e) const {
    throw "z3::uremFromGround Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  uremSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "z3::uremSymbolic Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  uremFromSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "z3::uremFromSymbolic Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  sremGround(const ExpressionImpGroundInteger *const e) const {
    throw "z3::sremGround Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  sremFromGround(const ExpressionImpGroundInteger *const e) const {
    throw "z3::sremFromGround Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  sremSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "z3::sremSymbolic Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  sremFromSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "z3::sremFromSymbolic Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  bitXorGround(const ExpressionImpGroundInteger *const e) const {
    throw "bitXorGround Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  bitXorSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "bitXorSymbolic Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  bitAndGround(const ExpressionImpGroundInteger *const e) const {
    throw "bitAndGround Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  bitAndSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "bitAndSymbolic Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  shiftLForGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpSymbolic::shiftLForGround N/A";
  }

  virtual ExpressionImp *
  shiftLGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpSymbolic::shiftLGround N/A";
  }

  virtual ExpressionImp *
  shiftLForSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpSymbolic::shiftLForSymbolic N/A";
  }

  virtual ExpressionImp *
  shiftLSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpSymbolic::shiftLSymbolic N/A";
  }

  virtual ExpressionImp *
  aShiftRForGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpSymbolic::ashiftRForGround N/A";
  }

  virtual ExpressionImp *
  aShiftRGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpSymbolic::ashiftRGround N/A";
  }

  virtual ExpressionImp *
  aShiftRForSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpSymbolic::ashiftRForSymbolic N/A";
  }

  virtual ExpressionImp *
  aShiftRSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpSymbolic::ashiftRSymbolic N/A";
  }

  virtual ExpressionImp *
  lShiftRForGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpSymbolic::lshiftRForGround N/A";
  }

  virtual ExpressionImp *
  lShiftRGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpSymbolic::lshiftRGround N/A";
  }

  virtual ExpressionImp *
  lShiftRForSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpSymbolic::lshiftRForSymbolic N/A";
  }

  virtual ExpressionImp *
  lShiftRSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpSymbolic::lshiftRSymbolic N/A";
  }

  virtual ExpressionImp *
  logicalAndGround(const ExpressionImpGroundBoolean *const e) const {
    SOLVER_EXPR_TYPE args[2];
    args[0] = this->_expr;
    args[1] = solverBooleanExprFromGround(e);
    SOLVER_EXPR_TYPE exp = Z3_mk_and(getSolver(), 2, args);
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *
  logicalAndSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpZ3 *const bexpr = llvm::dyn_cast<LAExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE args[2];
    args[0] = this->_expr;
    args[1] = bexpr->_expr;
    SOLVER_EXPR_TYPE exp = Z3_mk_and(getSolver(), 2, args);
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *
  bitOrGround(const ExpressionImpGroundInteger *const e) const {
    throw "Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  bitOrSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  logicalOrGround(const ExpressionImpGroundBoolean *const e) const {
    SOLVER_EXPR_TYPE args[2];
    args[0] = this->_expr;
    args[1] = solverBooleanExprFromGround(e);
    SOLVER_EXPR_TYPE exp = Z3_mk_or(getSolver(), 2, args);
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *
  logicalOrSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpZ3 *const bexpr = llvm::dyn_cast<LAExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE args[2];
    args[0] = this->_expr;
    args[1] = bexpr->_expr;
    SOLVER_EXPR_TYPE exp = Z3_mk_or(getSolver(), 2, args);
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *bitNot() const {
    throw "Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *logicalNot() const {
    SOLVER_EXPR_TYPE exp = Z3_mk_not(getSolver(), this->_expr);
    return new LAExpressionImpZ3(exp);

  }

  virtual ExpressionImp *
  ultGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_lt(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *
  ultSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpZ3 *const bexpr = llvm::dyn_cast<LAExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_lt(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpZ3(exp);
  }

  SOLVER_EXPR_TYPE solverExprFromUnknown(const ExpressionImp *const e) const {
    const ExpressionImpGroundInteger *const u =
        llvm::dyn_cast<ExpressionImpGroundInteger>(e);
    if (u != 0) {
      //FIXME
      SOLVER_EXPR_TYPE set = solverSignedExprFromGround(u);
      return set;
    }

    const ExpressionImpGroundBoolean *const b =
        llvm::dyn_cast<ExpressionImpGroundBoolean>(e);
    if (b != 0)
      return solverBooleanExprFromGround(b);

    const LAExpressionImpZ3 *const s = llvm::dyn_cast<LAExpressionImpZ3>(e);
    assert(s != 0);
    return s->_expr;
  }

  virtual ExpressionImp *array(const std::string &name, size_t domain,
                               size_t range) {
    Z3_sort z3domain = Z3_mk_int_sort(getSolver());
    Z3_sort z3range = Z3_mk_int_sort(getSolver());
    Z3_sort a = Z3_mk_array_sort(getSolver(), z3domain, z3range);
    Z3_symbol s = Z3_mk_string_symbol(getSolver(), name.c_str());
    SOLVER_EXPR_TYPE exp = Z3_mk_const(getSolver(), s, a);
    print(exp,a);

    return new LAExpressionImpZ3(exp); //, range, Z3_ARRAY);
  }

  virtual ExpressionImp *select(const ExpressionImp *const array,
                                const ExpressionImp *const index) const {
    const LAExpressionImpZ3 *const sarray =
        llvm::dyn_cast<LAExpressionImpZ3>(array);

    SOLVER_EXPR_TYPE exp =
        Z3_mk_select(getSolver(), sarray->_expr, solverExprFromUnknown(index));
    //FIXME ovo width nije jasno sta je
    return new LAExpressionImpZ3(exp); //, sarray->_width, Z3_BITVECTOR);
  }

  virtual ExpressionImp *store(const ExpressionImp *const array,
                               const ExpressionImp *const index,
                               const ExpressionImp *const value) const {
    const LAExpressionImpZ3 *sarray = llvm::dyn_cast<LAExpressionImpZ3>(array);
    SOLVER_EXPR_TYPE eindex = solverExprFromUnknown(index);
    SOLVER_EXPR_TYPE evalue = solverExprFromUnknown(value);
    SOLVER_EXPR_TYPE exp =
        Z3_mk_store(getSolver(), sarray->_expr, eindex, evalue);
    //FIXME ovo width nije jasno sta je
    return new LAExpressionImpZ3(exp); //, sarray->_width, Z3_BITVECTOR);

  }

  virtual ExpressionImp *
  uleGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_le(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *
  uleSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpZ3 *const bexpr = llvm::dyn_cast<LAExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_le(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *
  sltGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_lt(getSolver(), this->_expr, solverSignedExprFromGround(e));
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *
  sltSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpZ3 *const bexpr = llvm::dyn_cast<LAExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_lt(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *
  sleGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_le(getSolver(), this->_expr, solverSignedExprFromGround(e));
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *
  sleSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpZ3 *const bexpr = llvm::dyn_cast<LAExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_le(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *
  ugtGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_gt(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *
  ugtSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpZ3 *const bexpr = llvm::dyn_cast<LAExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_gt(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *
  ugeGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_ge(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *
  ugeSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpZ3 *const bexpr = llvm::dyn_cast<LAExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_ge(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *
  sgtGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_gt(getSolver(), this->_expr, solverSignedExprFromGround(e));
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *
  sgtSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpZ3 *const bexpr = llvm::dyn_cast<LAExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_gt(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *
  sgeGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_ge(getSolver(), this->_expr, solverSignedExprFromGround(e));
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *
  sgeSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpZ3 *const bexpr = llvm::dyn_cast<LAExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_ge(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *
  eqGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_eq(getSolver(), this->_expr, solverSignedExprFromGround(e));
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *
  eqGround(const ExpressionImpGroundBoolean *const e) const {
    SOLVER_EXPR_TYPE exp =
        Z3_mk_eq(getSolver(), this->_expr, solverBooleanExprFromGround(e));
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *
  eqSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpZ3 *const bexpr = llvm::dyn_cast<LAExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_eq(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *
  neqGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp = Z3_mk_not(
        getSolver(),
        Z3_mk_eq(getSolver(), solverSignedExprFromGround(e), this->_expr));
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *
  neqGround(const ExpressionImpGroundBoolean *const e) const {
    SOLVER_EXPR_TYPE exp = Z3_mk_not(
        getSolver(),
        Z3_mk_eq(getSolver(), solverBooleanExprFromGround(e), this->_expr));
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *
  neqSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpZ3 *const bexpr = llvm::dyn_cast<LAExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_not(
        getSolver(), Z3_mk_eq(getSolver(), this->_expr, bexpr->_expr));
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *ite_(ExpressionImp *thenExpr,
                              ExpressionImp *elseExpr) const {
    LAExpressionImpZ3 *bthenExpr = llvm::dyn_cast<LAExpressionImpZ3>(thenExpr);
    LAExpressionImpZ3 *belseExpr = llvm::dyn_cast<LAExpressionImpZ3>(elseExpr);
    assert(bthenExpr != 0 && belseExpr != 0);
    SOLVER_EXPR_TYPE exp =
        Z3_mk_ite(getSolver(), this->_expr, bthenExpr->_expr, belseExpr->_expr);
    return new LAExpressionImpZ3(exp);
  }

  virtual ExpressionImp *extractimp(size_t low, size_t high) const {
    throw "z3::ExpressionImpSymbolic::extractSymbolic N/A";
  }

  virtual ExpressionImp *zextimp(size_t width) const {
    throw "z3::ExpressionImpSymbolic::zextSymbolic N/A";
  }

  virtual ExpressionImp *sextimp(size_t width) const {
    throw "z3::ExpressionImpSymbolic::sextSymbolic N/A";
  }

  virtual void print(std::ostream &ostr) const {
    ostr << Z3_ast_to_string(getSolver(), _expr);
  }

private:

  void print(Z3_ast exp) const;
  void print(Z3_func_decl exp) const;
  void print(Z3_sort exp) const;
  void print(Z3_ast exp, Z3_sort sort) const;

  static SOLVER_TYPE getSolver() { return Z3Instance::instance().getSolver(); }

  SOLVER_EXPR_TYPE
  solverUnsignedExprFromGround(const ExpressionImpGround *const e) const {
    unsigned long value = e->getUnsignedValue();
    Z3_sort sort = Z3_mk_int_sort(getSolver());
    SOLVER_EXPR_TYPE exp =
        Z3_mk_numeral(getSolver(), toString(value).c_str(), sort);
    return exp;
  }

  SOLVER_EXPR_TYPE
  solverSignedExprFromGround(const ExpressionImpGround *const e) const {
    signed long value = e->getUnsignedValue();
    Z3_sort sort = Z3_mk_int_sort(getSolver());
    SOLVER_EXPR_TYPE exp =
        Z3_mk_numeral(getSolver(), toString((signed long) value).c_str(), sort);
    return exp;
  }

  SOLVER_EXPR_TYPE
  solverBooleanExprFromGround(const ExpressionImpGround *const e) const {
    unsigned value = e->getBooleanValue();
    return value ? Z3_mk_true(getSolver()) : Z3_mk_false(getSolver());
  }

  static SOLVER_TYPE _ctx;
  //  size_t _width;
  SOLVER_EXPR_TYPE _expr;
};

class LAExpressionFactoryZ3 : public ExpressionFactory {
public:
  LAExpressionFactoryZ3() : ExpressionFactory(new LAExpressionImpZ3()) {}
};

} // namespace UrsaMajor
#endif
