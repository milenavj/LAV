#include "config.h"
#ifdef Z3

#include "solvers/solver-interfaces/z3/bv-expression-z3.hpp"

#include "llvm/Support/CommandLine.h"
extern llvm::cl::opt<bool> CheckOverflow;
extern llvm::cl::opt<bool> DumpSMT;

namespace UrsaMajor {
thread_local std::map<std::string, Z3_func_decl>
    BVExpressionImpZ3::_uf_registry;

void BVExpressionImpZ3::print(Z3_ast exp) const {
    if(DumpSMT)
        std::cerr << Z3_ast_to_string (getSolver(), exp) << std::endl;
}

void BVExpressionImpZ3::print(Z3_func_decl f) const {
    if(DumpSMT)
        std::cerr << Z3_func_decl_to_string(getSolver(), f) << std::endl;
}
void BVExpressionImpZ3::print(Z3_sort sort) const {
    if(DumpSMT)
        std::cerr << Z3_sort_to_string(getSolver(), sort) << std::endl;
}

void BVExpressionImpZ3::print(Z3_ast expr, Z3_sort sort) const {
    if(!DumpSMT) return;
    std::cerr << "(declare-const "
              << std::string(Z3_ast_to_string(getSolver(), expr)) << " "
              << std::string(Z3_sort_to_string(getSolver(), sort))
              << ")"
              << std::endl;
}

ExpressionImp *
BVExpressionImpZ3::addGround(const ExpressionImpGroundInteger *const e) const {
  SOLVER_EXPR_TYPE exp = Z3_mk_bvadd(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
  return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
BVExpressionImpZ3::addOverflowGround(const ExpressionImpGroundInteger *const e) const {
  SOLVER_EXPR_TYPE exp =
      Z3_mk_bvadd_no_overflow(getSolver(), this->_expr, solverUnsignedExprFromGround(e), true);
  return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
BVExpressionImpZ3::addUnderflowGround(const ExpressionImpGroundInteger *const e) const {
  SOLVER_EXPR_TYPE exp =
      Z3_mk_bvadd_no_underflow(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
  return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
BVExpressionImpZ3::subOverflowGround(const ExpressionImpGroundInteger *const e) const {
  SOLVER_EXPR_TYPE exp =
      Z3_mk_bvsub_no_overflow(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
  return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
BVExpressionImpZ3::subUnderflowGround(const ExpressionImpGroundInteger *const e) const {
  SOLVER_EXPR_TYPE exp =
      Z3_mk_bvsub_no_underflow(getSolver(), this->_expr, solverUnsignedExprFromGround(e), true);
  return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
BVExpressionImpZ3::mulOverflowGround(const ExpressionImpGroundInteger *const e) const {
  SOLVER_EXPR_TYPE exp =
      Z3_mk_bvmul_no_overflow(getSolver(), this->_expr, solverUnsignedExprFromGround(e), true);
  return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
BVExpressionImpZ3::mulUnderflowGround(const ExpressionImpGroundInteger *const e) const {
  SOLVER_EXPR_TYPE exp =
      Z3_mk_bvmul_no_underflow(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
  return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
    BVExpressionImpZ3::mulOverflowSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvmul_no_overflow(getSolver(), this->_expr, bexpr->_expr, true);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
    BVExpressionImpZ3::mulUnderflowSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvmul_no_underflow(getSolver(), this->_expr, bexpr->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}



ExpressionImp *
BVExpressionImpZ3::sdivOverflowGround(const ExpressionImpGroundInteger *const e) const {
  SOLVER_EXPR_TYPE exp =
      Z3_mk_bvsdiv_no_overflow(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
  return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
    BVExpressionImpZ3::sdivOverflowSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvsdiv_no_overflow(getSolver(), this->_expr, bexpr->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
BVExpressionImpZ3::udivOverflowGround(const ExpressionImpGroundInteger *const e) const {
  SOLVER_EXPR_TYPE exp =
      Z3_mk_bvsdiv_no_overflow(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
  return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
    BVExpressionImpZ3::udivOverflowSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvsdiv_no_overflow(getSolver(), this->_expr, bexpr->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}


#define ENSURE(_x_)                                         \
    if (!(_x_)) {                                           \
        std::cerr << "Failed to verify: " << #_x_ << "\n";  \
        exit(-1);                                           \
    }

#define TEST(ctx, TEST_NAME, TEST_OUTCOME, NEG_TEST_OUTCOME) \
    do { \
        if (TEST_NAME != NULL) \
        { \
            Z3_solver_push(ctx, s);            \
            Z3_solver_assert(ctx, s, TEST_NAME);     \
            ENSURE(Z3_solver_check(ctx, s) == TEST_OUTCOME);     \
            Z3_solver_pop(ctx, s, 1);                              \
            \
            Z3_solver_push(ctx, s);                            \
            Z3_solver_assert(ctx, s, Z3_mk_not(ctx, TEST_NAME)); \
            ENSURE(Z3_solver_check(ctx, s) == NEG_TEST_OUTCOME);  \
            Z3_solver_pop(ctx, s, 1);                               \
        } \
    } while (0)

#define TEST_NO_OVERFLOW(ctx) TEST(ctx, test_ovfl, Z3_L_TRUE, Z3_L_FALSE)
#define TEST_OVERFLOW(ctx)    TEST(ctx, test_ovfl, Z3_L_FALSE, Z3_L_TRUE)

#define TEST_NO_UNDERFLOW TEST(test_udfl, Z3_L_TRUE, Z3_L_FALSE)
#define TEST_UNDERFLOW    TEST(test_udfl, Z3_L_FALSE, Z3_L_TRUE)


ExpressionImp *
    BVExpressionImpZ3::addOverflowSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvadd_no_overflow(getSolver(), this->_expr, bexpr->_expr, true);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
    BVExpressionImpZ3::addUnderflowSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvadd_no_underflow(getSolver(), this->_expr, bexpr->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
    BVExpressionImpZ3::subOverflowSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvsub_no_overflow(getSolver(), this->_expr, bexpr->_expr);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
    BVExpressionImpZ3::subUnderflowSymbolic(const ExpressionImpSymbolic *const e) const {
    const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
    assert(bexpr != 0);
    SOLVER_EXPR_TYPE exp = Z3_mk_bvsub_no_underflow(getSolver(), this->_expr, bexpr->_expr, true);
    return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}


ExpressionImp *
BVExpressionImpZ3::addSymbolic(const ExpressionImpSymbolic *const e) const {
  const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
  assert(bexpr != 0);
  SOLVER_EXPR_TYPE exp = Z3_mk_bvadd(getSolver(), this->_expr, bexpr->_expr);
  return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
BVExpressionImpZ3::subtractGround(const ExpressionImpGroundInteger *const e) const {
  SOLVER_EXPR_TYPE exp =
      Z3_mk_bvsub(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
  return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
BVExpressionImpZ3::subtractFromGround(const ExpressionImpGroundInteger *const e) const {
  SOLVER_EXPR_TYPE exp =
      Z3_mk_bvsub(getSolver(), solverUnsignedExprFromGround(e), this->_expr);
return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
BVExpressionImpZ3::subtractSymbolic(const ExpressionImpSymbolic *const e) const {
  const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
  assert(bexpr != 0);
  SOLVER_EXPR_TYPE exp = Z3_mk_bvsub(getSolver(), this->_expr, bexpr->_expr);
  return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
BVExpressionImpZ3::subtractFromSymbolic(const ExpressionImpSymbolic *const e) const {
  const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
  assert(bexpr != 0);
  SOLVER_EXPR_TYPE exp = Z3_mk_bvsub(getSolver(), bexpr->_expr, this->_expr);
  return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
BVExpressionImpZ3::multGround(const ExpressionImpGroundInteger *const e) const {
  SOLVER_EXPR_TYPE exp =
      Z3_mk_bvmul(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
  return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
BVExpressionImpZ3::multSymbolic(const ExpressionImpSymbolic *const e) const {
  const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
  assert(bexpr != 0);
  SOLVER_EXPR_TYPE exp = Z3_mk_bvmul(getSolver(), this->_expr, bexpr->_expr);
  return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
BVExpressionImpZ3::udivGround(const ExpressionImpGroundInteger *const e) const {
  SOLVER_EXPR_TYPE exp =
      Z3_mk_bvudiv(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
  return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
BVExpressionImpZ3::udivFromGround(const ExpressionImpGroundInteger *const e) const {
  SOLVER_EXPR_TYPE exp =
      Z3_mk_bvudiv(getSolver(), solverUnsignedExprFromGround(e), this->_expr);
  return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
BVExpressionImpZ3::udivSymbolic(const ExpressionImpSymbolic *const e) const {
  const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
  assert(bexpr != 0);
  SOLVER_EXPR_TYPE exp = Z3_mk_bvudiv(getSolver(), this->_expr, bexpr->_expr);
  return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
BVExpressionImpZ3::udivFromSymbolic(const ExpressionImpSymbolic *const e) const {
  const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
  assert(bexpr != 0);
  SOLVER_EXPR_TYPE exp = Z3_mk_bvudiv(getSolver(), bexpr->_expr, this->_expr);
  return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
BVExpressionImpZ3::sdivGround(const ExpressionImpGroundInteger *const e) const {
  SOLVER_EXPR_TYPE exp =
      Z3_mk_bvsdiv(getSolver(), this->_expr, solverSignedExprFromGround(e));
  return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
BVExpressionImpZ3::sdivFromGround(const ExpressionImpGroundInteger *const e) const {
  SOLVER_EXPR_TYPE exp =
      Z3_mk_bvsdiv(getSolver(), solverSignedExprFromGround(e), this->_expr);
  return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
BVExpressionImpZ3::sdivSymbolic(const ExpressionImpSymbolic *const e) const {
  const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
  assert(bexpr != 0);
  SOLVER_EXPR_TYPE exp = Z3_mk_bvsdiv(getSolver(), this->_expr, bexpr->_expr);
  return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
BVExpressionImpZ3::sdivFromSymbolic(const ExpressionImpSymbolic *const e) const {
  const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
  assert(bexpr != 0);
  SOLVER_EXPR_TYPE exp = Z3_mk_bvsdiv(getSolver(), bexpr->_expr, this->_expr);
  return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
BVExpressionImpZ3::uremGround(const ExpressionImpGroundInteger *const e) const {
  SOLVER_EXPR_TYPE exp =
      Z3_mk_bvurem(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
  return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
BVExpressionImpZ3::uremFromGround(const ExpressionImpGroundInteger *const e) const {
  SOLVER_EXPR_TYPE exp =
      Z3_mk_bvurem(getSolver(), solverUnsignedExprFromGround(e), this->_expr);
  return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
BVExpressionImpZ3::uremSymbolic(const ExpressionImpSymbolic *const e) const {
  const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
  assert(bexpr != 0);
  SOLVER_EXPR_TYPE exp = Z3_mk_bvurem(getSolver(), this->_expr, bexpr->_expr);
  return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
BVExpressionImpZ3::uremFromSymbolic(const ExpressionImpSymbolic *const e) const {
  const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
  assert(bexpr != 0);
  SOLVER_EXPR_TYPE exp = Z3_mk_bvurem(getSolver(), bexpr->_expr, this->_expr);
  return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
BVExpressionImpZ3::sremGround(const ExpressionImpGroundInteger *const e) const {
  SOLVER_EXPR_TYPE exp =
      Z3_mk_bvsrem(getSolver(), this->_expr, solverSignedExprFromGround(e));
  return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
BVExpressionImpZ3::sremFromGround(const ExpressionImpGroundInteger *const e) const {
  SOLVER_EXPR_TYPE exp =
      Z3_mk_bvsrem(getSolver(), solverSignedExprFromGround(e), this->_expr);
  return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
BVExpressionImpZ3::sremSymbolic(const ExpressionImpSymbolic *const e) const {
  const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
  assert(bexpr != 0);
  SOLVER_EXPR_TYPE exp = Z3_mk_bvsrem(getSolver(), this->_expr, bexpr->_expr);
  return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}

ExpressionImp *
BVExpressionImpZ3::sremFromSymbolic(const ExpressionImpSymbolic *const e) const {
  const BVExpressionImpZ3 *const bexpr = llvm::dyn_cast<BVExpressionImpZ3>(e);
  assert(bexpr != 0);
  SOLVER_EXPR_TYPE exp = Z3_mk_bvsrem(getSolver(), bexpr->_expr, this->_expr);
  return new BVExpressionImpZ3(exp, _width, Z3_BITVECTOR);
}





} // namespace UrsaMajor

#endif
