#if !defined __LA_EXPRESSION_MATHSAT__
#define __LA_EXPRESSION_MATHSAT__

#include <sstream>

#include "../expression.hpp"
#include "mathsat-instance.hpp"
#include <assert.h>

namespace UrsaMajor {
class LAExpressionImpMathSAT : public ExpressionImpSymbolic {
public:
  typedef msat_term SOLVER_EXPR_TYPE;
  typedef msat_env SOLVER_TYPE;
  enum MathSATType {
    MATHSAT_BOOLEAN,
    MATHSAT_BITVECTOR
  };

  explicit LAExpressionImpMathSAT()
      : ExpressionImpSymbolic(EK_ExpressionImpMathSAT) {}

  explicit LAExpressionImpMathSAT(SOLVER_EXPR_TYPE expr)
      : ExpressionImpSymbolic(EK_ExpressionImpMathSAT), _expr(expr) {}

  static bool classof(const ExpressionImp *S) {
    return S->getKind() == EK_ExpressionImpMathSAT;
  }

  bool checkSat() {
    std::cout << "Mathsat::searching..." << std::endl;
    MathSATInstance::instance().push();
    return MathSATInstance::instance().nextModel(_expr);
  }

  bool addTempConstraint() {
    return MathSATInstance::instance().addTempConstraint(_expr);
    return true;
  }

  bool addConstraint() {
    return MathSATInstance::instance().addConstraint(_expr);
    return true;
  }

  void reset() {
    std::cout << "Reseting context..." << std::endl;
    //      MathSATInstance::instance().pop();
    MathSATInstance::instance().reset();
  }

  std::string getAssignment() const {
    return MathSATInstance::instance().getAssignment(_expr);
  }

  virtual ExpressionImp *
      uninterpretedFunction(const Function &fun,
                            const std::vector<const ExpressionImp *> &args);

  ExpressionImp *unsignedSymbolic(const std::string &name, size_t width) const {
    msat_decl decl = msat_declare_variable(
        getSolver(), const_cast<char *>(name.c_str()), MSAT_INT);
    SOLVER_EXPR_TYPE exp = msat_make_variable(getSolver(), decl);

    return new LAExpressionImpMathSAT(exp);
  }

  ExpressionImp *signedSymbolic(const std::string &name, size_t width) const {
    msat_decl decl = msat_declare_variable(
        getSolver(), const_cast<char *>(name.c_str()), MSAT_INT);
    SOLVER_EXPR_TYPE exp = msat_make_variable(getSolver(), decl);
    return new LAExpressionImpMathSAT(exp);
  }

  ExpressionImp *integerGround(unsigned x, size_t width) const {
    return new LAExpressionImpMathSAT(solverUnsignedExprFromGround(x, width));
  }

  ExpressionImp *integerGround(signed x, size_t width) const {
    return new LAExpressionImpMathSAT(solverSignedExprFromGround(x, width));
  }

  ExpressionImp *integerGround(unsigned long x, size_t width) const {
    return new LAExpressionImpMathSAT(solverUnsignedExprFromGround(x, width));
  }

  ExpressionImp *integerGround(signed long x, size_t width) const {
    return new LAExpressionImpMathSAT(solverSignedExprFromGround(x, width));
  }

  ExpressionImp *integerGround(unsigned short x, size_t width) const {
    return new LAExpressionImpMathSAT(solverUnsignedExprFromGround(x, width));
  }

  ExpressionImp *integerGround(signed short x, size_t width) const {
    return new LAExpressionImpMathSAT(solverSignedExprFromGround(x, width));
  }

  ExpressionImp *booleanSymbolic(const std::string &name) const {
    msat_decl decl = msat_declare_variable(
        getSolver(), const_cast<char *>(name.c_str()), MSAT_BOOL);
    SOLVER_EXPR_TYPE exp = msat_make_variable(getSolver(), decl);
    return new LAExpressionImpMathSAT(exp);
  }

  ExpressionImp *booleanGround(bool x) const {
    return new LAExpressionImpMathSAT(solverBooleanExprFromGround(x));
  }

  virtual ExpressionImp *
  addGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp = msat_make_plus(getSolver(), this->_expr,
                                          solverUnsignedExprFromGround(e));
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  addSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpMathSAT *const bexpr =
        llvm::dyn_cast<LAExpressionImpMathSAT>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        msat_make_plus(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  subtractGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp = msat_make_minus(getSolver(), this->_expr,
                                           solverUnsignedExprFromGround(e));
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  subtractFromGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp = msat_make_minus(
        getSolver(), solverUnsignedExprFromGround(e), this->_expr);
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  subtractSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpMathSAT *const bexpr =
        llvm::dyn_cast<LAExpressionImpMathSAT>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        msat_make_minus(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  subtractFromSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpMathSAT *const bexpr =
        llvm::dyn_cast<LAExpressionImpMathSAT>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        msat_make_minus(getSolver(), bexpr->_expr, this->_expr);
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  multGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp = msat_make_times(getSolver(), this->_expr,
                                           solverUnsignedExprFromGround(e));
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  multSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpMathSAT *const bexpr =
        llvm::dyn_cast<LAExpressionImpMathSAT>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        msat_make_times(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  udivGround(const ExpressionImpGroundInteger *const e) const {
    throw "mathsat::udivGround Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  udivFromGround(const ExpressionImpGroundInteger *const e) const {
    throw "mathsat::udivFromGround Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  udivSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "mathsat::udivSymbolic Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  udivFromSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "mathsat::udivFromSymbolic Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  sdivGround(const ExpressionImpGroundInteger *const e) const {
    throw "mathsat::sdivGround Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  sdivFromGround(const ExpressionImpGroundInteger *const e) const {
    throw "mathsat::sdivFromGround Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  sdivSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "mathsat::sdivSymbolic Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  sdivFromSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "mathsat::sdivFromSymbolic Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  uremGround(const ExpressionImpGroundInteger *const e) const {
    throw "mathsat::uremGround Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  uremFromGround(const ExpressionImpGroundInteger *const e) const {
    throw "mathsat:: Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  uremSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "mathsat:: Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  uremFromSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "mathsat:: Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  sremGround(const ExpressionImpGroundInteger *const e) const {
    throw "mathsat:: Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  sremFromGround(const ExpressionImpGroundInteger *const e) const {
    throw "mathsat:: Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  sremSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "mathsat:: Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  sremFromSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "mathsat:: Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  bitXorGround(const ExpressionImpGroundInteger *const e) const {
    throw "mathsat:: Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  bitXorSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "mathsat:: Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  bitAndGround(const ExpressionImpGroundInteger *const e) const {
    throw "mathsat:: Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  bitAndSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "mathsat:: Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  logicalAndGround(const ExpressionImpGroundBoolean *const e) const {
    SOLVER_EXPR_TYPE exp =
        msat_make_and(getSolver(), this->_expr, solverBooleanExprFromGround(e));
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  logicalAndSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpMathSAT *const bexpr =
        llvm::dyn_cast<LAExpressionImpMathSAT>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        msat_make_and(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  bitOrGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        msat_make_or(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  bitOrSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpMathSAT *const bexpr =
        llvm::dyn_cast<LAExpressionImpMathSAT>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = msat_make_or(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  logicalOrGround(const ExpressionImpGroundBoolean *const e) const {
    SOLVER_EXPR_TYPE exp =
        msat_make_or(getSolver(), this->_expr, solverBooleanExprFromGround(e));
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  logicalOrSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpMathSAT *const bexpr =
        llvm::dyn_cast<LAExpressionImpMathSAT>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = msat_make_or(getSolver(), _expr, bexpr->_expr);
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *bitNot() const {
    SOLVER_EXPR_TYPE exp = msat_make_not(getSolver(), this->_expr);
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *logicalNot() const {
    SOLVER_EXPR_TYPE exp = msat_make_not(getSolver(), this->_expr);
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  shiftLForGround(const ExpressionImpGroundInteger *const e) const {
    throw "mathsat:: Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  shiftLGround(const ExpressionImpGroundInteger *const e) const {
    throw "mathsat:: Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  shiftLForSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "mathsat:: Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  shiftLSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "mathsat:: Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  lShiftRForGround(const ExpressionImpGroundInteger *const e) const {
    throw "mathsat:: Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  lShiftRGround(const ExpressionImpGroundInteger *const e) const {
    throw "mathsat:: Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  lShiftRSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "mathsat:: Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  lShiftRForSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "mathsat:: Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  aShiftRForGround(const ExpressionImpGroundInteger *const e) const {
    throw "mathsat:: Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  aShiftRGround(const ExpressionImpGroundInteger *const e) const {
    throw "mathsat:: Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  aShiftRSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "mathsat:: Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  aShiftRForSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "mathsat:: Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *
  ultGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        msat_make_lt(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  ultSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpMathSAT *const bexpr =
        llvm::dyn_cast<LAExpressionImpMathSAT>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = msat_make_lt(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  uleGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp = msat_make_leq(getSolver(), this->_expr,
                                         solverUnsignedExprFromGround(e));
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  uleSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpMathSAT *const bexpr =
        llvm::dyn_cast<LAExpressionImpMathSAT>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        msat_make_leq(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  sltGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        msat_make_lt(getSolver(), this->_expr, solverSignedExprFromGround(e));
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  sltSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpMathSAT *const bexpr =
        llvm::dyn_cast<LAExpressionImpMathSAT>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = msat_make_lt(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  sleGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        msat_make_leq(getSolver(), this->_expr, solverSignedExprFromGround(e));
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  sleSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpMathSAT *const bexpr =
        llvm::dyn_cast<LAExpressionImpMathSAT>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        msat_make_leq(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  ugtGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        msat_make_gt(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  ugtSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpMathSAT *const bexpr =
        llvm::dyn_cast<LAExpressionImpMathSAT>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = msat_make_gt(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  ugeGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp = msat_make_geq(getSolver(), this->_expr,
                                         solverUnsignedExprFromGround(e));
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  ugeSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpMathSAT *const bexpr =
        llvm::dyn_cast<LAExpressionImpMathSAT>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        msat_make_geq(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  sgtGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        msat_make_gt(getSolver(), this->_expr, solverSignedExprFromGround(e));
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  sgtSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpMathSAT *const bexpr =
        llvm::dyn_cast<LAExpressionImpMathSAT>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = msat_make_gt(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  sgeGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        msat_make_geq(getSolver(), this->_expr, solverSignedExprFromGround(e));
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  sgeSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpMathSAT *const bexpr =
        llvm::dyn_cast<LAExpressionImpMathSAT>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        msat_make_geq(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpMathSAT(exp);
  }

  //FIXME unsigned???
  virtual ExpressionImp *
  eqGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp = msat_make_equal(getSolver(), this->_expr,
                                           solverSignedExprFromGround(e));
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  eqGround(const ExpressionImpGroundBoolean *const e) const {
    SOLVER_EXPR_TYPE exp = msat_make_equal(getSolver(), this->_expr,
                                           solverBooleanExprFromGround(e));
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  eqSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpMathSAT *const bexpr =
        llvm::dyn_cast<LAExpressionImpMathSAT>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        msat_make_equal(getSolver(), this->_expr, bexpr->_expr);
    return new LAExpressionImpMathSAT(exp);
  }

  //FIXME unsigned???
  virtual ExpressionImp *
  neqGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp = msat_make_not(
        getSolver(), msat_make_equal(getSolver(), this->_expr,
                                     solverSignedExprFromGround(e)));
    return new LAExpressionImpMathSAT(exp);
  }

  //FIXME reci filipu da je ovo menjano
  virtual ExpressionImp *
  neqGround(const ExpressionImpGroundBoolean *const e) const {
    SOLVER_EXPR_TYPE exp = msat_make_not(
        getSolver(), msat_make_equal(getSolver(), this->_expr,
                                     solverBooleanExprFromGround(e)));
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *
  neqSymbolic(const ExpressionImpSymbolic *const e) const {
    const LAExpressionImpMathSAT *const bexpr =
        llvm::dyn_cast<LAExpressionImpMathSAT>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = msat_make_not(
        getSolver(), msat_make_equal(getSolver(), this->_expr, bexpr->_expr));
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *ite_(ExpressionImp *thenExpr,
                              ExpressionImp *elseExpr) const {
    LAExpressionImpMathSAT *bthenExpr =
        llvm::dyn_cast<LAExpressionImpMathSAT>(thenExpr);
    LAExpressionImpMathSAT *belseExpr =
        llvm::dyn_cast<LAExpressionImpMathSAT>(elseExpr);
    assert(bthenExpr != 0 && belseExpr != 0);
    SOLVER_EXPR_TYPE exp = msat_make_ite(getSolver(), this->_expr,
                                         bthenExpr->_expr, belseExpr->_expr);
    return new LAExpressionImpMathSAT(exp);
  }

  virtual ExpressionImp *sextimp(size_t width) const {
    throw "mathsat:: Nonlinear arithmetic is not supported!";
  }

  virtual ExpressionImp *zextimp(size_t width) const {
    throw "mathsat:: Nonlinear arithmetic is not supported!";
  }

  //FIXME ovo mozda postoji, proveriti
  virtual ExpressionImp *extractimp(size_t low, size_t high) const {
    throw "mathsat::ExpressionImpSymbolic::extractSymbolic N/A";
  }

  virtual void print(std::ostream &ostr) const {
    char *s = msat_term_repr(_expr);
    ostr << s;
    free(s);
  }

private:
  static SOLVER_TYPE getSolver() {
    return MathSATInstance::instance().getSolver();
  }

  SOLVER_EXPR_TYPE solverUnsignedExprFromGround(
      const ExpressionImpGroundInteger *const e) const {
    unsigned value = e->getUnsignedValue();
    std::stringstream s;
    s << value;
    return msat_make_number(getSolver(), s.str().c_str());
  }

  SOLVER_EXPR_TYPE solverUnsignedExprFromGround(unsigned long x,
                                                size_t width) const {
    std::stringstream s;
    //      s << "0d" << width << "_" << x;
    s << x;
    return msat_make_number(getSolver(), s.str().c_str());
  }

  SOLVER_EXPR_TYPE
  solverSignedExprFromGround(const ExpressionImpGroundInteger *const e) const {
    signed value = e->getUnsignedValue();
    std::stringstream s;
    s << value;
    return msat_make_number(getSolver(), s.str().c_str());
  }

  SOLVER_EXPR_TYPE solverSignedExprFromGround(unsigned long x,
                                              size_t width) const {
    std::stringstream s;
    s << (signed long) x;
    return msat_make_number(getSolver(), s.str().c_str());
  }

  SOLVER_EXPR_TYPE
  solverBooleanExprFromGround(const ExpressionImpGround *const e) const {
    return solverBooleanExprFromGround(e->getBooleanValue());
  }

  SOLVER_EXPR_TYPE solverBooleanExprFromGround(bool x) const {
    return x ? msat_make_true(getSolver()) : msat_make_false(getSolver());
  }

  SOLVER_EXPR_TYPE _expr;
  size_t _width;
  MathSATType _type;
};

class LAExpressionFactoryMathSAT : public ExpressionFactory {
public:
  LAExpressionFactoryMathSAT()
      : ExpressionFactory(new LAExpressionImpMathSAT()) {}
};

} // namespace UrsaMajor

#endif
