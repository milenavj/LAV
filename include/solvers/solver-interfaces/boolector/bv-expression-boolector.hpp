#if !defined __BV_EXPRESSION_BOOLECTOR__
#define __BV_EXPRESSION_BOOLECTOR__

#include <cassert>
#include <cstdlib>
#include "../expression.hpp"
#include "boolector-instance.hpp"
#include <stdio.h>

namespace UrsaMajor {
// ---------------------------------------------------------------------------
class BVExpressionImpBoolector : public ExpressionImpSymbolic {
public:
  typedef Btor *SOLVER_TYPE;

  explicit BVExpressionImpBoolector()
      : ExpressionImpSymbolic(EK_ExpressionImpBoolector), _expr(0) {}

  explicit BVExpressionImpBoolector(SOLVER_EXPR_TYPE expr)
      : ExpressionImpSymbolic(EK_ExpressionImpBoolector), _expr(expr) {}

  static bool classof(const ExpressionImp *S) {
    return S->getKind() == EK_ExpressionImpBoolector;
  }

  ~BVExpressionImpBoolector() {}

  bool checkSat() {
    // std::cout << "Boolector::searching..." << std::endl;
    // boolector_dump_smt(getSolver(), stdout, _expr);
    return BoolectorInstance::instance().nextModel(_expr);
  }

  bool addConstraint() {
    // std::cout << "Boolector::addConstraint()..." << std::endl;
    // boolector_dump_smt(getSolver(), stdout, _expr);
    return BoolectorInstance::instance().addConstraint(_expr);
  }

  bool addTempConstraint() {
    // std::cout << "Boolector::addTempConstraint()..." << std::endl;
    // boolector_dump_smt(getSolver(), stdout, _expr);
    return BoolectorInstance::instance().addTempConstraint(_expr);
  }

  void reset() {
    // std::cout << "Boolector::reseting..." << std::endl;
    BoolectorInstance::instance().reset();
  }

  std::string getAssignment() const {
    return BoolectorInstance::instance().getAssignment(_expr);
  }

  ExpressionImp *unsignedSymbolic(const std::string &name, size_t width) const {
    SOLVER_EXPR_TYPE exp = boolector_var(getSolver(), width, name.c_str());
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  ExpressionImp *signedSymbolic(const std::string &name, size_t width) const {
    SOLVER_EXPR_TYPE exp = boolector_var(getSolver(), width, name.c_str());
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  ExpressionImp *booleanSymbolic(const std::string &name) const {
    return unsignedSymbolic(name, 1);
  }

  ExpressionImp *integerGround(unsigned x, size_t width) const {
    SOLVER_EXPR_TYPE exp = boolector_unsigned_int(getSolver(), x, width);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  ExpressionImp *integerGround(signed x, size_t width) const {
    SOLVER_EXPR_TYPE exp = boolector_int(getSolver(), x, width);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  ExpressionImp *integerGround(unsigned long x, size_t width) const {
    SOLVER_EXPR_TYPE exp = boolector_unsigned_int(getSolver(), x, width);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  ExpressionImp *integerGround(signed long x, size_t width) const {
    SOLVER_EXPR_TYPE exp = boolector_int(getSolver(), x, width);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  ExpressionImp *integerGround(unsigned short x, size_t width) const {
    SOLVER_EXPR_TYPE exp = boolector_unsigned_int(getSolver(), x, width);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  ExpressionImp *integerGround(signed short x, size_t width) const {
    SOLVER_EXPR_TYPE exp = boolector_int(getSolver(), x, width);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  ExpressionImp *booleanGround(bool x) const { return integerGround(x, 1); }

  virtual ExpressionImp *array(const std::string &name, size_t domain,
                               size_t range) {
    SOLVER_EXPR_TYPE array =
        boolector_array(getSolver(), range, domain, name.c_str());
    BoolectorInstance::instance().ExpToRelease(array);
    return new BVExpressionImpBoolector(array);
  }

  virtual ExpressionImp *select(const ExpressionImp *const array,
                                const ExpressionImp *const index) const {
    const BVExpressionImpBoolector *const sarray =
        llvm::dyn_cast<BVExpressionImpBoolector>(array);

    SOLVER_EXPR_TYPE exp = boolector_read(getSolver(), sarray->_expr,
                                          solverExprFromUnknown(index));
    BoolectorInstance::instance().ExpToRelease(exp);

    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *store(const ExpressionImp *const array,
                               const ExpressionImp *const index,
                               const ExpressionImp *const value) const {
    const BVExpressionImpBoolector *const sarray =
        llvm::dyn_cast<BVExpressionImpBoolector>(array);

    SOLVER_EXPR_TYPE bindex = solverExprFromUnknown(index);
    SOLVER_EXPR_TYPE bvalue = solverExprFromUnknown(value);
    SOLVER_EXPR_TYPE exp =
        boolector_write(getSolver(), sarray->_expr, bindex, bvalue);
    BoolectorInstance::instance().ExpToRelease(exp);

    return new BVExpressionImpBoolector(exp);
  }

  // FIXME: remove case split
  SOLVER_EXPR_TYPE solverExprFromUnknown(const ExpressionImp *const e) const {
    const ExpressionImpGroundInteger *u =
        llvm::dyn_cast<ExpressionImpGroundInteger>(e);
    if (u != 0)
      //FIXME proveriti da li je signed ili unsigned
      return solverUnsignedExprFromGround(u);

    const ExpressionImpGroundBoolean *b =
        llvm::dyn_cast<ExpressionImpGroundBoolean>(e);
    if (b != 0)
      return solverBooleanExprFromGround(b);

    const BVExpressionImpBoolector *s =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(s != 0);
    return s->_expr;
  }

  virtual ExpressionImp *
  addGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp = boolector_add(getSolver(), this->_expr,
                                         solverUnsignedExprFromGround(e));
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  addSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpBoolector *const bexpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        boolector_add(getSolver(), this->_expr, bexpr->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  subtractGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp = boolector_sub(getSolver(), this->_expr,
                                         solverUnsignedExprFromGround(e));
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  subtractFromGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp = boolector_sub(
        getSolver(), solverUnsignedExprFromGround(e), this->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  subtractSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpBoolector *const bexpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        boolector_sub(getSolver(), this->_expr, bexpr->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  subtractFromSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpBoolector *const bexpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        boolector_sub(getSolver(), bexpr->_expr, this->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  multGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp = boolector_mul(getSolver(), this->_expr,
                                         solverUnsignedExprFromGround(e));
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  multSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpBoolector *const bexpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        boolector_mul(getSolver(), this->_expr, bexpr->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  udivGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp = boolector_udiv(getSolver(), this->_expr,
                                          solverUnsignedExprFromGround(e));
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  udivFromGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp = boolector_udiv(
        getSolver(), solverUnsignedExprFromGround(e), this->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  udivSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpBoolector *const bexpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        boolector_udiv(getSolver(), this->_expr, bexpr->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  udivFromSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpBoolector *const bexpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        boolector_udiv(getSolver(), bexpr->_expr, this->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  sdivGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        boolector_sdiv(getSolver(), this->_expr, solverSignedExprFromGround(e));
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  sdivFromGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        boolector_sdiv(getSolver(), solverSignedExprFromGround(e), this->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  sdivSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpBoolector *const bexpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        boolector_sdiv(getSolver(), this->_expr, bexpr->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  sdivFromSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpBoolector *const bexpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        boolector_sdiv(getSolver(), bexpr->_expr, this->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  uremGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp = boolector_urem(getSolver(), this->_expr,
                                          solverUnsignedExprFromGround(e));
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  uremFromGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp = boolector_urem(
        getSolver(), solverUnsignedExprFromGround(e), this->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  uremSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpBoolector *const bexpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        boolector_urem(getSolver(), this->_expr, bexpr->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  uremFromSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpBoolector *const bexpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        boolector_urem(getSolver(), bexpr->_expr, this->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  sremGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        boolector_srem(getSolver(), this->_expr, solverSignedExprFromGround(e));
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  sremFromGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        boolector_srem(getSolver(), solverSignedExprFromGround(e), this->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  sremSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpBoolector *const bexpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        boolector_srem(getSolver(), this->_expr, bexpr->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  sremFromSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpBoolector *const bexpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        boolector_srem(getSolver(), bexpr->_expr, this->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  bitXorGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp = boolector_xor(getSolver(), this->_expr,
                                         solverUnsignedExprFromGround(e));
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  bitXorSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpBoolector *const bexpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        boolector_xor(getSolver(), this->_expr, bexpr->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  bitAndGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp = boolector_and(getSolver(), this->_expr,
                                         solverUnsignedExprFromGround(e));
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  bitAndSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpBoolector *const bexpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        boolector_and(getSolver(), this->_expr, bexpr->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  logicalAndGround(const ExpressionImpGroundBoolean *const e) const {
    SOLVER_EXPR_TYPE exp =
        boolector_and(getSolver(), this->_expr, solverBooleanExprFromGround(e));
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  logicalAndSymbolic(const ExpressionImpSymbolic *const e) const {
    return bitAndSymbolic(e);
  }

  virtual ExpressionImp *
  bitOrGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        boolector_or(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  bitOrSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpBoolector *const bexpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = boolector_or(getSolver(), this->_expr, bexpr->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  logicalOrGround(const ExpressionImpGroundBoolean *const e) const {
    SOLVER_EXPR_TYPE exp =
        boolector_or(getSolver(), this->_expr, solverBooleanExprFromGround(e));
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  logicalOrSymbolic(const ExpressionImpSymbolic *const e) const {
    return bitOrSymbolic(e);
  }

  virtual ExpressionImp *bitNot() const {
    SOLVER_EXPR_TYPE exp = boolector_not(getSolver(), this->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *logicalNot() const {
    SOLVER_EXPR_TYPE exp = boolector_not(getSolver(), this->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  shiftLForGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp = boolector_sll(
        getSolver(), this->_expr,
        solverUnsignedExprFromGround(
            e, mylog2(boolector_get_width(getSolver(), this->_expr))));
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  shiftLGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp = boolector_sll(
        getSolver(), solverUnsignedExprFromGround(e), this->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  shiftLForSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpBoolector *const bexpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = boolector_sll(getSolver(), _expr, bexpr->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);

  }

  virtual ExpressionImp *
  shiftLSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpBoolector *const bexpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = boolector_sll(getSolver(), bexpr->_expr, _expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  lShiftRForGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp = boolector_srl(
        getSolver(), this->_expr,
        solverUnsignedExprFromGround(
            e, mylog2(boolector_get_width(getSolver(), this->_expr))));
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  lShiftRGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp = boolector_srl(
        getSolver(), solverUnsignedExprFromGround(e), this->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  lShiftRSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpBoolector *const bexpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = boolector_srl(getSolver(), bexpr->_expr, _expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  lShiftRForSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpBoolector *const bexpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = boolector_srl(getSolver(), _expr, bexpr->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  aShiftRForGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp = boolector_srl(
        getSolver(), this->_expr,
        solverSignedExprFromGround(
            e, mylog2(boolector_get_width(getSolver(), this->_expr))));
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  aShiftRGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        boolector_srl(getSolver(), solverSignedExprFromGround(e), this->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  aShiftRSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpBoolector *const bexpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = boolector_srl(getSolver(), bexpr->_expr, _expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  aShiftRForSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpBoolector *const bexpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = boolector_srl(getSolver(), _expr, bexpr->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  ultGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp = boolector_ult(getSolver(), this->_expr,
                                         solverUnsignedExprFromGround(e));
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  ultSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpBoolector *const bexpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        boolector_ult(getSolver(), this->_expr, bexpr->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  uleGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp = boolector_ulte(getSolver(), this->_expr,
                                          solverUnsignedExprFromGround(e));
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  uleSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpBoolector *const bexpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        boolector_ulte(getSolver(), this->_expr, bexpr->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  sltGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        boolector_slt(getSolver(), this->_expr, solverSignedExprFromGround(e));
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  sltSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpBoolector *const bexpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        boolector_slt(getSolver(), this->_expr, bexpr->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  sleGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        boolector_slte(getSolver(), this->_expr, solverSignedExprFromGround(e));
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  sleSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpBoolector *const bexpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        boolector_slte(getSolver(), this->_expr, bexpr->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  ugtGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp = boolector_ugt(getSolver(), this->_expr,
                                         solverUnsignedExprFromGround(e));
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  ugtSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpBoolector *const bexpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        boolector_ugt(getSolver(), this->_expr, bexpr->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  ugeGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp = boolector_ugte(getSolver(), this->_expr,
                                          solverUnsignedExprFromGround(e));
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  ugeSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpBoolector *const bexpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        boolector_ugte(getSolver(), this->_expr, bexpr->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  sgtGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        boolector_sgt(getSolver(), this->_expr, solverSignedExprFromGround(e));
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  sgtSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpBoolector *const bexpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        boolector_sgt(getSolver(), this->_expr, bexpr->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  sgeGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        boolector_sgte(getSolver(), this->_expr, solverSignedExprFromGround(e));
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  sgeSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpBoolector *const bexpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp =
        boolector_sgte(getSolver(), this->_expr, bexpr->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  eqGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        boolector_eq(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  eqGround(const ExpressionImpGroundBoolean *const e) const {
    SOLVER_EXPR_TYPE exp =
        boolector_eq(getSolver(), this->_expr, solverBooleanExprFromGround(e));
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  eqSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpBoolector *const bexpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = boolector_eq(getSolver(), this->_expr, bexpr->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  neqGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp =
        boolector_ne(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  neqGround(const ExpressionImpGroundBoolean *const e) const {
    SOLVER_EXPR_TYPE exp =
        boolector_ne(getSolver(), this->_expr, solverBooleanExprFromGround(e));
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *
  neqSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpBoolector *const bexpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = boolector_ne(getSolver(), this->_expr, bexpr->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *ite_(ExpressionImp *thenExpr,
                              ExpressionImp *elseExpr) const {
    BVExpressionImpBoolector *bthenExpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(thenExpr);
    BVExpressionImpBoolector *belseExpr =
        llvm::dyn_cast<BVExpressionImpBoolector>(elseExpr);
    assert(bthenExpr != 0 && belseExpr != 0);
    SOLVER_EXPR_TYPE exp = boolector_cond(getSolver(), this->_expr,
                                          bthenExpr->_expr, belseExpr->_expr);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual ExpressionImp *zextimp(size_t width) const {
    int size = width - boolector_get_width(getSolver(), this->_expr);
    if (size > 0) {
      SOLVER_EXPR_TYPE exp = boolector_uext(getSolver(), this->_expr, size);
      BoolectorInstance::instance().ExpToRelease(exp);
      return new BVExpressionImpBoolector(exp);
    } else
      return new BVExpressionImpBoolector(this->_expr);
  }

  virtual ExpressionImp *sextimp(size_t width) const {
    int size = width - boolector_get_width(getSolver(), this->_expr);
    if (size > 0) {
      SOLVER_EXPR_TYPE exp =
          boolector_sext(getSolver(), this->_expr,
                         width - boolector_get_width(getSolver(), this->_expr));
      BoolectorInstance::instance().ExpToRelease(exp);
      return new BVExpressionImpBoolector(exp);
    } else
      return new BVExpressionImpBoolector(this->_expr);
  }

  virtual ExpressionImp *extractimp(size_t low, size_t high) const {
    SOLVER_EXPR_TYPE exp = boolector_slice(getSolver(), this->_expr, high, low);
    BoolectorInstance::instance().ExpToRelease(exp);
    return new BVExpressionImpBoolector(exp);
  }

  virtual void print(std::ostream &ostr) const {
    FILE *file = tmpfile();
#if defined ENABLE_BOOLECTOR
    boolector_dump_btor_node(getSolver(), file, _expr);
#endif
#if defined ENABLE_BOOLECTOR_OLD
    boolector_dump_btor(getSolver(), file, _expr);
#endif
    rewind(file);
    char buff[8192];
    int size = 0;
    while ((size = fread(buff, 1, 8192, file)) > 0)
      ostr.write(buff, size);
    fclose(file);
  }

  static SOLVER_TYPE getSolver() {
    return BoolectorInstance::instance().getSolver();
  }

private:
  static unsigned mylog2(unsigned val) {
    unsigned ret = -1;
    while (val != 0) {
      val >>= 1;
      ret++;
    }
    return ret;
  }

  SOLVER_EXPR_TYPE
  solverUnsignedExprFromGround(const ExpressionImpGroundInteger *const e,
                               size_t width) const {
    unsigned long value = e->getUnsignedValue();
    SOLVER_EXPR_TYPE exp = boolector_unsigned_int(getSolver(), value, width);
    BoolectorInstance::instance().ExpToRelease(exp);
    return exp;
  }

  SOLVER_EXPR_TYPE solverUnsignedExprFromGround(
      const ExpressionImpGroundInteger *const e) const {
    return solverUnsignedExprFromGround(e, e->GetWidth());
  }

  SOLVER_EXPR_TYPE
  solverSignedExprFromGround(const ExpressionImpGroundInteger *const e,
                             size_t width) const {
    unsigned long value = e->getUnsignedValue();
    SOLVER_EXPR_TYPE exp = boolector_int(getSolver(), value, width);
    BoolectorInstance::instance().ExpToRelease(exp);
    return exp;
  }

  SOLVER_EXPR_TYPE
  solverSignedExprFromGround(const ExpressionImpGroundInteger *const e) const {
    SOLVER_EXPR_TYPE exp = solverSignedExprFromGround(
        e, boolector_get_width(getSolver(), this->_expr));
    BoolectorInstance::instance().ExpToRelease(exp);
    return exp;
  }

  SOLVER_EXPR_TYPE
  solverBooleanExprFromGround(const ExpressionImpGroundBoolean *const e) const {
    bool value = e->getBooleanValue();
    SOLVER_EXPR_TYPE exp = boolector_unsigned_int(getSolver(), value, 1);
    BoolectorInstance::instance().ExpToRelease(exp);
    return exp;
  }

  SOLVER_EXPR_TYPE _expr;
};

// ---------------------------------------------------------------------------
class BVExpressionFactoryBoolector : public ExpressionFactory {
public:
  BVExpressionFactoryBoolector()
      : ExpressionFactory(new BVExpressionImpBoolector()) {}
};

} // namespace UrsaMajor

#endif
