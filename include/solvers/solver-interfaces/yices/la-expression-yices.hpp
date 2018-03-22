#if !defined __LA_EXPRESSION_YICES__
#define __LA_EXPRESSION_YICES__

#include <cassert>
#include <map>

#include "../expression.hpp"
#include "yices-instance.hpp"

extern "C" {
#include "yices_c.h"
}

namespace UrsaMajor {

class LAExpressionImpYices : public ExpressionImpSymbolic {
public:
  typedef yices_expr SOLVER_EXPR_TYPE;
  typedef yices_context SOLVER_TYPE;

  LAExpressionImpYices()
      : ExpressionImpSymbolic(EK_ExpressionImpYices), _expr(0) {}

  explicit LAExpressionImpYices(SOLVER_EXPR_TYPE expr)
      : ExpressionImpSymbolic(EK_ExpressionImpYices), _expr(expr) {}

  static bool classof(const ExpressionImp *S) {
    return S->getKind() == EK_ExpressionImpYices;
  }

  ~LAExpressionImpYices() {}

  bool checkSat() {
    std::cout << "Yices::searching..." << std::endl;
    return YicesInstance::instance().nextModel(_expr);
  }

  bool addTempConstraint() {
    return YicesInstance::instance().addTempConstraint(_expr);
  }

  bool addConstraint() {
    return YicesInstance::instance().addConstraint(_expr);
  }

  void reset() {
    std::cout << "Reseting context..." << std::endl;
    YicesInstance::instance().reset();
  }

  std::string getAssignment() const {
    // TODO: FIXME: getIntAssignment vs getBooleanAssignment
    return YicesInstance::instance().getIntAssignment(_expr);
  }

  LAExpressionImpYices *unsignedSymbolic(const std::string &name,
                                         size_t width) const {
    yices_type type = yices_mk_type(getSolver(), const_cast<char *>("nat"));
    yices_var_decl decl =
        yices_mk_var_decl(getSolver(), const_cast<char *>(name.c_str()), type);
    SOLVER_EXPR_TYPE exp = yices_mk_var_from_decl(getSolver(), decl);
    return new LAExpressionImpYices(exp);
  }

  LAExpressionImpYices *signedSymbolic(const std::string &name,
                                       size_t width) const {
    yices_type type = yices_mk_type(getSolver(), const_cast<char *>("int"));
    yices_var_decl decl =
        yices_mk_var_decl(getSolver(), const_cast<char *>(name.c_str()), type);
    SOLVER_EXPR_TYPE exp = yices_mk_var_from_decl(getSolver(), decl);
    return new LAExpressionImpYices(exp);
  }

  LAExpressionImpYices *booleanSymbolic(const std::string &name) const {
    SOLVER_EXPR_TYPE exp = yices_mk_fresh_bool_var(getSolver());
    return new LAExpressionImpYices(exp);
  }

  LAExpressionImpYices *integerGround(unsigned int x, size_t width) const {
    SOLVER_EXPR_TYPE exp = yices_mk_num(getSolver(), x);
    return new LAExpressionImpYices(exp);
  }

  LAExpressionImpYices *integerGround(signed int x, size_t width) const {
    SOLVER_EXPR_TYPE exp = yices_mk_num(getSolver(), x);
    return new LAExpressionImpYices(exp);
  }

  LAExpressionImpYices *integerGround(unsigned long int x, size_t width) const {
    SOLVER_EXPR_TYPE exp = yices_mk_num(getSolver(), x);
    return new LAExpressionImpYices(exp);
  }

  LAExpressionImpYices *integerGround(signed long int x, size_t width) const {
    SOLVER_EXPR_TYPE exp = yices_mk_num(getSolver(), x);
    return new LAExpressionImpYices(exp);
  }

  LAExpressionImpYices *integerGround(unsigned short int x,
                                      size_t width) const {
    SOLVER_EXPR_TYPE exp = yices_mk_num(getSolver(), x);
    return new LAExpressionImpYices(exp);
  }

  LAExpressionImpYices *integerGround(signed short int x, size_t width) const {
    SOLVER_EXPR_TYPE exp = yices_mk_num(getSolver(), x);
    return new LAExpressionImpYices(exp);
  }

  LAExpressionImpYices *booleanGround(bool x) const {
    SOLVER_EXPR_TYPE exp =
        x ? yices_mk_true(getSolver()) : yices_mk_false(getSolver());
    return new LAExpressionImpYices(exp);
  }

  yices_type translateType(Type t) {
    yices_type unsigned_type =
        yices_mk_type(getSolver(), const_cast<char *>("int"));
    yices_type bool_type =
        yices_mk_type(getSolver(), const_cast<char *>("bool"));
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
    //static std::map<std::string, yices_expr> _uf_registry;
    std::map<std::string, yices_expr> &_uf_registry =
        YicesInstance::instance()._uf_registry;

    SOLVER_EXPR_TYPE f;
    if (_uf_registry.find(fun.getName()) == _uf_registry.end()) {
      yices_type *domain_types = new yices_type[n];
      for (size_t k = 0; k < n; k++)
        domain_types[k] = translateType(fun.getArgumentType(k));
      yices_type result_type = translateType(fun.getType());
      yices_type fty =
          yices_mk_function_type(getSolver(), domain_types, n, result_type);
      delete[] domain_types;

      yices_var_decl fdecl = yices_mk_var_decl(
          getSolver(), const_cast<char *>(fun.getName().c_str()), fty);
      f = yices_mk_var_from_decl(getSolver(), fdecl);
      _uf_registry[fun.getName()] = f;
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
      const LAExpressionImpYices *y = llvm::dyn_cast<LAExpressionImpYices>(*i);
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
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *
  addGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE args[2];
    args[0] = this->_expr;
    args[1] = solverUnsignedExprFromGround(e);
    SOLVER_EXPR_TYPE exp = yices_mk_sum(getSolver(), args, 2);
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *
  addSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpYices *const bexpr =
        llvm::dyn_cast<LAExpressionImpYices>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE args[2];
    args[0] = this->_expr;
    args[1] = bexpr->_expr;
    SOLVER_EXPR_TYPE exp = yices_mk_sum(getSolver(), args, 2);
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *
  addOverflowGround(const ExpressionImpGroundInteger *const e) const {
      throw "yices::addOverflowGround not supported!";
  }

  virtual ExpressionImp *
  addOverflowSymbolic(const ExpressionImpSymbolic *const e) const {
      throw "yices::addOverflowSymbolic not supported!";
  }

  virtual ExpressionImp *
  subtractGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE args[2];
    args[0] = this->_expr;
    args[1] = solverUnsignedExprFromGround(e);
    SOLVER_EXPR_TYPE exp = yices_mk_sub(getSolver(), args, 2);
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *
  subtractFromGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE args[2];
    args[0] = solverUnsignedExprFromGround(e);
    args[1] = this->_expr;
    SOLVER_EXPR_TYPE exp = yices_mk_sub(getSolver(), args, 2);
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *
  subtractSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpYices *const bexpr =
        llvm::dyn_cast<LAExpressionImpYices>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE args[2];
    args[0] = this->_expr;
    args[1] = bexpr->_expr;
    SOLVER_EXPR_TYPE exp = yices_mk_sub(getSolver(), args, 2);
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *
  subtractFromSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpYices *const bexpr =
        llvm::dyn_cast<LAExpressionImpYices>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE args[2];
    args[0] = bexpr->_expr;
    args[1] = this->_expr;
    SOLVER_EXPR_TYPE exp = yices_mk_sub(getSolver(), args, 2);
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *
  multGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE args[2];
    args[0] = this->_expr;
    args[1] = solverUnsignedExprFromGround(e);
    SOLVER_EXPR_TYPE exp = yices_mk_mul(getSolver(), args, 2);
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *array(const std::string &name, size_t domain,
                               size_t range) {
    throw "yices --- arrays not supported!";
  }
  virtual ExpressionImp *select(const ExpressionImp *const array,
                                const ExpressionImp *const index) const {
    throw "yices --- arrays not supported!";
  }
  virtual ExpressionImp *store(const ExpressionImp *const array,
                               const ExpressionImp *const index,
                               const ExpressionImp *const value) const {
    throw "yices --- arrays not supported!";
  }
  virtual ExpressionImp *
  multSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  udivGround(const ExpressionImpGroundInteger *const e) const {
    throw "yices::Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  udivFromGround(const ExpressionImpGroundInteger *const e) const {
    throw "yices::Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  udivSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "yices::Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  udivFromSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "yices::Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  sdivGround(const ExpressionImpGroundInteger *const e) const {
    throw "yices::Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  sdivFromGround(const ExpressionImpGroundInteger *const e) const {
    throw "yices::Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  sdivSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "yices::Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  sdivFromSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "yices::Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  uremGround(const ExpressionImpGroundInteger *const e) const {
    throw "yices::Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  uremFromGround(const ExpressionImpGroundInteger *const e) const {
    throw "yices::Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  uremSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "yices::Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  uremFromSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "yices::Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  sremGround(const ExpressionImpGroundInteger *const e) const {
    throw "yices::Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  sremFromGround(const ExpressionImpGroundInteger *const e) const {
    throw "yices::Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  sremSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "yices::Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  sremFromSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "yices::Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  bitXorGround(const ExpressionImpGroundInteger *const e) const {
    throw "Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  bitXorSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  bitAndGround(const ExpressionImpGroundInteger *const e) const {
    throw "Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  bitAndSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "Nonlinear arithmetic is not supported!";
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
    SOLVER_EXPR_TYPE exp = yices_mk_and(getSolver(), args, 2);
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *
  logicalAndSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpYices *const bexpr =
        llvm::dyn_cast<LAExpressionImpYices>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE args[2];
    args[0] = this->_expr;
    args[1] = bexpr->_expr;
    SOLVER_EXPR_TYPE exp = yices_mk_and(getSolver(), args, 2);
    return new LAExpressionImpYices(exp);
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
    SOLVER_EXPR_TYPE exp = yices_mk_or(getSolver(), args, 2);
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *
  logicalOrSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpYices *const bexpr =
        llvm::dyn_cast<LAExpressionImpYices>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE args[2];
    args[0] = this->_expr;
    args[1] = bexpr->_expr;
    SOLVER_EXPR_TYPE exp = yices_mk_or(getSolver(), args, 2);
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *bitNot() const {
    throw "Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *logicalNot() const {
    SOLVER_EXPR_TYPE exp = yices_mk_not(getSolver(), this->_expr);
    return new LAExpressionImpYices(exp);

  }

  virtual ExpressionImp *
  ultGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        yices_mk_lt(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *
  ultSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpYices *const bexpr =
        llvm::dyn_cast<LAExpressionImpYices>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = yices_mk_lt(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *
  uleGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        yices_mk_le(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *
  uleSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpYices *const bexpr =
        llvm::dyn_cast<LAExpressionImpYices>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = yices_mk_le(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpYices(exp);
  }
  //FIXME da li ovde treba slt-sle-sgt-sge a yices to nema ili nije ni vazno
  virtual ExpressionImp *
  sltGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        yices_mk_lt(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *
  sltSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpYices *const bexpr =
        llvm::dyn_cast<LAExpressionImpYices>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = yices_mk_lt(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *
  sleGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        yices_mk_le(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *
  sleSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpYices *const bexpr =
        llvm::dyn_cast<LAExpressionImpYices>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = yices_mk_le(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *
  ugtGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        yices_mk_gt(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *
  ugtSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpYices *const bexpr =
        llvm::dyn_cast<LAExpressionImpYices>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = yices_mk_gt(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *
  ugeGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        yices_mk_ge(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *
  ugeSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpYices *const bexpr =
        llvm::dyn_cast<LAExpressionImpYices>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = yices_mk_ge(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *
  sgtGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        yices_mk_gt(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *
  sgtSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpYices *const bexpr =
        llvm::dyn_cast<LAExpressionImpYices>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = yices_mk_gt(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *
  sgeGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        yices_mk_ge(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *
  sgeSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpYices *const bexpr =
        llvm::dyn_cast<LAExpressionImpYices>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = yices_mk_ge(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *
  eqGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        yices_mk_eq(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *
  eqGround(const ExpressionImpGroundBoolean *const e) const {
    SOLVER_EXPR_TYPE exp =
        yices_mk_eq(getSolver(), this->_expr, solverBooleanExprFromGround(e));
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *
  eqSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpYices *const bexpr =
        llvm::dyn_cast<LAExpressionImpYices>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = yices_mk_eq(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *
  neqGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp = yices_mk_diseq(getSolver(), this->_expr,
                                          solverUnsignedExprFromGround(e));
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *
  neqGround(const ExpressionImpGroundBoolean *const e) const {
    SOLVER_EXPR_TYPE exp = yices_mk_diseq(getSolver(), this->_expr,
                                          solverBooleanExprFromGround(e));
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *
  neqSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpYices *const bexpr =
        llvm::dyn_cast<LAExpressionImpYices>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        yices_mk_diseq(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *ite_(ExpressionImp *thenExpr,
                              ExpressionImp *elseExpr) const {
    LAExpressionImpYices *bthenExpr =
        llvm::dyn_cast<LAExpressionImpYices>(thenExpr);
    LAExpressionImpYices *belseExpr =
        llvm::dyn_cast<LAExpressionImpYices>(elseExpr);
    assert(bthenExpr != 0 && belseExpr != 0);
    SOLVER_EXPR_TYPE exp = yices_mk_ite(getSolver(), this->_expr,
                                        bthenExpr->_expr, belseExpr->_expr);
    return new LAExpressionImpYices(exp);
  }

  virtual ExpressionImp *zextimp(size_t width) const {
    //    virtual ExpressionImp* zextSymbolic(size_t width) const {
    throw "yices::ExpressionImpSymbolic::zextSymbolic N/A";
  }

  virtual ExpressionImp *sextimp(size_t width) const {
    //    virtual ExpressionImp* sextSymbolic(size_t width) const {
    throw "yices::ExpressionImpSymbolic::sextSymbolic N/A";
  }

  virtual ExpressionImp *extractimp(size_t low, size_t high) const {
    throw "yices::ExpressionImpSymbolic::extractSymbolic N/A";
  }

  virtual void print(std::ostream &ostr) const { yices_pp_expr(_expr); }

private:
  static SOLVER_TYPE getSolver() {
    /*      if (_ctx == 0) {
    	_ctx = yices_mk_context();
    	yices_enable_type_checker(true);
          }
          return _ctx;
    */
    return YicesInstance::instance().getSolver();
  }

  SOLVER_EXPR_TYPE
  solverUnsignedExprFromGround(const ExpressionImpGround *const e) const {
    unsigned long value = e->getUnsignedValue();
    return yices_mk_num(getSolver(), value);
  }

  SOLVER_EXPR_TYPE
  solverBooleanExprFromGround(const ExpressionImpGround *const e) const {
    bool value = e->getBooleanValue();
    return value ? yices_mk_true(getSolver()) : yices_mk_false(getSolver());
  }

  //    static SOLVER_TYPE _ctx;
  size_t _width;
  SOLVER_EXPR_TYPE _expr;
};

class LAExpressionFactoryYices : public ExpressionFactory {
public:
  LAExpressionFactoryYices() : ExpressionFactory(new LAExpressionImpYices()) {}
};

} // namespace UrsaMajor
#endif
