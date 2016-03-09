#if !defined __BV_EXPRESSION_MATHSAT__
#define __BV_EXPRESSION_MATHSAT__

#include <sstream>

#include "../expression.hpp"
#include "mathsat-instance.hpp"
#include <assert.h>

namespace UrsaMajor {
  class BVExpressionImpMathSAT : public ExpressionImpSymbolic {
  public:
    typedef msat_term SOLVER_EXPR_TYPE;
    typedef msat_env SOLVER_TYPE;
    enum MathSATType {MATHSAT_BOOLEAN, MATHSAT_BITVECTOR};


    explicit BVExpressionImpMathSAT() : ExpressionImpSymbolic(EK_ExpressionImpMathSAT) {}

    explicit BVExpressionImpMathSAT(SOLVER_EXPR_TYPE expr, size_t width, MathSATType type) 
      : ExpressionImpSymbolic(EK_ExpressionImpMathSAT), _expr(expr), _width(width), _type(type) {
    }

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

    virtual ExpressionImp* uninterpretedFunction(const Function& fun, const std::vector<const ExpressionImp*>& args);

    ExpressionImp* unsignedSymbolic(const std::string& name, size_t width) const {
      msat_decl decl = msat_declare_variable(getSolver(), const_cast<char*>(name.c_str()), MSAT_BV + width);
      SOLVER_EXPR_TYPE exp = msat_make_variable(getSolver(), decl);

      return new BVExpressionImpMathSAT(exp, width, MATHSAT_BITVECTOR);
    }

//FIXME
    ExpressionImp* signedSymbolic(const std::string& name, size_t width) const {
      msat_decl decl = msat_declare_variable(getSolver(), const_cast<char*>(name.c_str()), MSAT_BV + width);
      SOLVER_EXPR_TYPE exp = msat_make_variable(getSolver(), decl);

      return new BVExpressionImpMathSAT(exp, width, MATHSAT_BITVECTOR);
    }


    ExpressionImp* integerGround(unsigned x, size_t width) const {
      return new BVExpressionImpMathSAT(solverUnsignedExprFromGround(x, width), width, MATHSAT_BITVECTOR);
    }

    ExpressionImp* integerGround(signed x, size_t width) const {
      return new BVExpressionImpMathSAT(solverSignedExprFromGround(x, width), width, MATHSAT_BITVECTOR);
    }

    ExpressionImp* integerGround(unsigned long x, size_t width) const {
      return new BVExpressionImpMathSAT(solverUnsignedExprFromGround(x, width), width, MATHSAT_BITVECTOR);
    }

    ExpressionImp* integerGround(signed long x, size_t width) const {
      return new BVExpressionImpMathSAT(solverSignedExprFromGround(x, width), width, MATHSAT_BITVECTOR);
    }

    ExpressionImp* integerGround(unsigned short x, size_t width) const {
      return new BVExpressionImpMathSAT(solverUnsignedExprFromGround(x, width), width, MATHSAT_BITVECTOR);
    }

    ExpressionImp* integerGround(signed short x, size_t width) const {
      return new BVExpressionImpMathSAT(solverSignedExprFromGround(x, width), width, MATHSAT_BITVECTOR);
    }

    ExpressionImp* booleanSymbolic(const std::string& name) const {
      msat_decl decl = msat_declare_variable(getSolver(), const_cast<char*>(name.c_str()), MSAT_BOOL);
      SOLVER_EXPR_TYPE exp = msat_make_variable(getSolver(), decl);
      return new BVExpressionImpMathSAT(exp, 1, MATHSAT_BOOLEAN);
    }

    ExpressionImp* booleanGround(bool x) const {
      return new BVExpressionImpMathSAT(solverBooleanExprFromGround(x), 1, MATHSAT_BOOLEAN);
    }

    virtual ExpressionImp* addGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
	msat_make_bv_plus(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* addSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
	llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE exp = msat_make_bv_plus(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* subtractGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
	msat_make_bv_minus(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* subtractFromGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
	msat_make_bv_minus(getSolver(), solverUnsignedExprFromGround(e), this->_expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* subtractSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
	llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE exp = 
	msat_make_bv_minus(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* subtractFromSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
	llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE exp = 
	msat_make_bv_minus(getSolver(), bexpr->_expr, this->_expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* multGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
	msat_make_bv_times(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* multSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
	llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE exp = 
	msat_make_bv_times(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }







    virtual ExpressionImp* udivGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_udiv(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* udivFromGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_udiv(getSolver(), solverUnsignedExprFromGround(e), this->_expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* udivSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
        llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_udiv(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* udivFromSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
        llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_udiv(getSolver(), bexpr->_expr, this->_expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* sdivGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_sdiv(getSolver(), this->_expr, solverSignedExprFromGround(e));
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* sdivFromGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_sdiv(getSolver(), solverSignedExprFromGround(e), this->_expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* sdivSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
        llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_sdiv(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* sdivFromSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
        llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_sdiv(getSolver(), bexpr->_expr, this->_expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }




    virtual ExpressionImp* uremGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_urem(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* uremFromGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_urem(getSolver(), solverUnsignedExprFromGround(e), this->_expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* uremSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
        llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_urem(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* uremFromSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
        llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_urem(getSolver(), bexpr->_expr, this->_expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* sremGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_srem(getSolver(), this->_expr, solverSignedExprFromGround(e));
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* sremFromGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_srem(getSolver(), solverSignedExprFromGround(e), this->_expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* sremSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
        llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_srem(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* sremFromSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
        llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_srem(getSolver(), bexpr->_expr, this->_expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }








    virtual ExpressionImp* bitXorGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
	msat_make_bv_xor(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* bitXorSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
	llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE exp = 
	msat_make_bv_xor(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* bitAndGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
	msat_make_bv_and(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* bitAndSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
	llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE exp = 
	msat_make_bv_and(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* logicalAndGround(const ExpressionImpGroundBoolean* const e) const {
      SOLVER_EXPR_TYPE exp = 
	msat_make_and(getSolver(), this->_expr, solverBooleanExprFromGround(e));
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BOOLEAN);
    }

    virtual ExpressionImp* logicalAndSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
	llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE exp = 
	msat_make_and(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BOOLEAN);
    }

    virtual ExpressionImp* bitOrGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
	msat_make_bv_or(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* bitOrSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
	llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE exp = 
	msat_make_bv_or(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* logicalOrGround(const ExpressionImpGroundBoolean* const e) const {
      SOLVER_EXPR_TYPE exp = 
	msat_make_or(getSolver(), this->_expr, solverBooleanExprFromGround(e));
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BOOLEAN);
    }

    virtual ExpressionImp* logicalOrSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
	llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE exp = 
	msat_make_or(getSolver(), _expr, bexpr->_expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BOOLEAN);
    }


    virtual ExpressionImp* bitNot() const {
      SOLVER_EXPR_TYPE exp = 
	msat_make_bv_not(getSolver(), this->_expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* logicalNot() const {
      SOLVER_EXPR_TYPE exp = 
	msat_make_not(getSolver(), this->_expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BOOLEAN);
    }

    virtual ExpressionImp* shiftLForGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
	msat_make_bv_lsl(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* shiftLGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
	msat_make_bv_lsl(getSolver(), solverUnsignedExprFromGround(e), this->_expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* shiftLForSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
	llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE exp = 
	msat_make_bv_lsl(getSolver(), _expr, bexpr->_expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);

    }

    virtual ExpressionImp* shiftLSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
	llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE exp = 
	msat_make_bv_lsl(getSolver(), bexpr->_expr, _expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* lShiftRForGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
	msat_make_bv_lsr(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* lShiftRGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
	msat_make_bv_lsr(getSolver(), solverUnsignedExprFromGround(e), this->_expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* lShiftRSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
	llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE exp = 
	msat_make_bv_lsr(getSolver(), bexpr->_expr, _expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* lShiftRForSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
	llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE exp = 
	msat_make_bv_lsr(getSolver(), _expr, bexpr->_expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* aShiftRForGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_asr(getSolver(), this->_expr, solverSignedExprFromGround(e));
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* aShiftRGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_asr(getSolver(), solverSignedExprFromGround(e), this->_expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* aShiftRSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
        llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_asr(getSolver(), bexpr->_expr, _expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* aShiftRForSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
        llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_asr(getSolver(), _expr, bexpr->_expr);
      return new BVExpressionImpMathSAT(exp, _width, MATHSAT_BITVECTOR);
    }


    virtual ExpressionImp* ultGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
	msat_make_bv_ult(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
      return new BVExpressionImpMathSAT(exp, 1, MATHSAT_BOOLEAN);
    }

    virtual ExpressionImp* ultSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
	llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);      
      SOLVER_EXPR_TYPE exp = 
	msat_make_bv_ult(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpMathSAT(exp, 1, MATHSAT_BOOLEAN);
    }

    virtual ExpressionImp* uleGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_uleq(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
      return new BVExpressionImpMathSAT(exp, 1, MATHSAT_BOOLEAN);
    }

    virtual ExpressionImp* uleSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
        llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);      
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_uleq(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpMathSAT(exp, 1, MATHSAT_BOOLEAN);
    }

    virtual ExpressionImp* sltGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_slt(getSolver(), this->_expr, solverSignedExprFromGround(e));
      return new BVExpressionImpMathSAT(exp, 1, MATHSAT_BOOLEAN);
    }

    virtual ExpressionImp* sltSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
        llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);      
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_slt(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpMathSAT(exp, 1, MATHSAT_BOOLEAN);
    }

    virtual ExpressionImp* sleGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_sleq(getSolver(), this->_expr, solverSignedExprFromGround(e));
      return new BVExpressionImpMathSAT(exp, 1, MATHSAT_BOOLEAN);
    }

    virtual ExpressionImp* sleSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
        llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);      
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_sleq(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpMathSAT(exp, 1, MATHSAT_BOOLEAN);
    }


    virtual ExpressionImp* ugtGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
	msat_make_bv_ugt(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
      return new BVExpressionImpMathSAT(exp, 1, MATHSAT_BOOLEAN);
    }

    virtual ExpressionImp* ugtSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
	llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);      
      SOLVER_EXPR_TYPE exp = 
	msat_make_bv_ugt(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpMathSAT(exp, 1, MATHSAT_BOOLEAN);
    }

    virtual ExpressionImp* ugeGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_ugeq(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
      return new BVExpressionImpMathSAT(exp, 1, MATHSAT_BOOLEAN);
    }

    virtual ExpressionImp* ugeSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
        llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);      
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_ugeq(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpMathSAT(exp, 1, MATHSAT_BOOLEAN);
    }

    virtual ExpressionImp* sgtGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_sgt(getSolver(), this->_expr, solverSignedExprFromGround(e));
      return new BVExpressionImpMathSAT(exp, 1, MATHSAT_BOOLEAN);
    }

    virtual ExpressionImp* sgtSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
        llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);      
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_sgt(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpMathSAT(exp, 1, MATHSAT_BOOLEAN);
    }

    virtual ExpressionImp* sgeGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_sgeq(getSolver(), this->_expr, solverSignedExprFromGround(e));
      return new BVExpressionImpMathSAT(exp, 1, MATHSAT_BOOLEAN);
    }

    virtual ExpressionImp* sgeSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
        llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);      
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_sgeq(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpMathSAT(exp, 1, MATHSAT_BOOLEAN);
    }


    virtual ExpressionImp* eqGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
	msat_make_equal(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
      return new BVExpressionImpMathSAT(exp, 1, MATHSAT_BOOLEAN);
    }

    virtual ExpressionImp* eqGround(const ExpressionImpGroundBoolean* const e) const {
      SOLVER_EXPR_TYPE exp = 
	msat_make_equal(getSolver(), this->_expr, solverBooleanExprFromGround(e));
      return new BVExpressionImpMathSAT(exp, 1, MATHSAT_BOOLEAN);
    }

    virtual ExpressionImp* eqSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
	llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);      
      SOLVER_EXPR_TYPE exp = 
	msat_make_equal(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpMathSAT(exp, 1, MATHSAT_BOOLEAN);
    }

    virtual ExpressionImp* neqGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
	msat_make_not(getSolver(), 
		      msat_make_equal(getSolver(), this->_expr, solverUnsignedExprFromGround(e)));
      return new BVExpressionImpMathSAT(exp, 1, MATHSAT_BOOLEAN);
    }

//FIXME reci filipu da je ovo menjano
    virtual ExpressionImp* neqGround(const ExpressionImpGroundBoolean* const e) const {
      SOLVER_EXPR_TYPE exp = 
	msat_make_not(getSolver(),
		      msat_make_equal(getSolver(), this->_expr, solverBooleanExprFromGround(e)));
      return new BVExpressionImpMathSAT(exp, 1, MATHSAT_BOOLEAN);
    }

    virtual ExpressionImp* neqSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpMathSAT* const bexpr = 
	llvm::dyn_cast<BVExpressionImpMathSAT>(e);
      assert(bexpr != 0);      
      SOLVER_EXPR_TYPE exp = 
	msat_make_not(getSolver(),
		      msat_make_equal(getSolver(), this->_expr, bexpr->_expr));
      return new BVExpressionImpMathSAT(exp, 1, MATHSAT_BOOLEAN);
    }

    virtual ExpressionImp* ite_(ExpressionImp* thenExpr,
			    ExpressionImp* elseExpr) const {
      BVExpressionImpMathSAT* bthenExpr = 
	llvm::dyn_cast<BVExpressionImpMathSAT>(thenExpr);
      BVExpressionImpMathSAT*  belseExpr =
	llvm::dyn_cast<BVExpressionImpMathSAT>(elseExpr);
      assert(bthenExpr != 0 && belseExpr != 0);
      SOLVER_EXPR_TYPE exp = 
	msat_make_ite(getSolver(), this->_expr, bthenExpr->_expr, belseExpr->_expr);
      return new BVExpressionImpMathSAT(exp, bthenExpr->_width, MATHSAT_BITVECTOR);
    }


    virtual ExpressionImp* sextimp(size_t width) const {
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_sext(getSolver(), this->_expr, width);
      return new BVExpressionImpMathSAT(exp, width, MATHSAT_BITVECTOR);
    }

    virtual ExpressionImp* zextimp(size_t width) const {
      SOLVER_EXPR_TYPE exp = 
        msat_make_bv_zext(getSolver(), this->_expr, width);
      return new BVExpressionImpMathSAT(exp, width, MATHSAT_BITVECTOR);
    }

//FIXME ovo mozda postoji, proveriti
    virtual ExpressionImp* extractimp(size_t low, size_t high) const {
      throw "mathsat::ExpressionImpSymbolic::extractSymbolic N/A";
    }

    virtual void print(std::ostream& ostr) const {
      char* s = msat_term_repr(_expr);
      ostr << s;
      free(s);
    }

  private:
    static SOLVER_TYPE getSolver() {
      return MathSATInstance::instance().getSolver();
    }

    SOLVER_EXPR_TYPE solverUnsignedExprFromGround(const ExpressionImpGroundInteger* const e) const {
      unsigned value = e->getUnsignedValue();
      std::stringstream s;
//FIXME reci filipu, ovde je bilo _width a ja sam promenila u e->getwidth - i gore sam promenila tip
//i u klasu sam ubacila getwidth jer se _width nije postavljalo u ovom slucaju i pucalo je
//pojavi se i greska kod neq
      s << "0d" << e->GetWidth() << "_" << value;
      return msat_make_number(getSolver(), s.str().c_str()); 
//      return solverUnsignedExprFromGround(e->getUnsignedValue(), _width);
    }

    SOLVER_EXPR_TYPE solverUnsignedExprFromGround(unsigned long x, size_t width) const {
      std::stringstream s;
      s << "0d" << width << "_" << x;
      return msat_make_number(getSolver(), s.str().c_str()); 
    }

    SOLVER_EXPR_TYPE solverSignedExprFromGround(const ExpressionImpGroundInteger* const e) const {
//FIXME trebalo bi signed
      unsigned value = e->getUnsignedValue();
      std::stringstream s;
//FIXME reci filipu, ovde je bilo _width a ja sam promenila u e->getwidth - i gore sam promenila tip
//i u klasu sam ubacila getwidth jer se _width nije postavljalo u ovom slucaju i pucalo je
//pojavi se i greska kod neq
      s << "0d" << e->GetWidth() << "_" << value;
      return msat_make_number(getSolver(), s.str().c_str()); 
//      return solverUnsignedExprFromGround(e->getUnsignedValue(), _width);

//      std::cout << "ovde je greska 1    " << _width << std::endl;
//      return solverSignedExprFromGround(e->getUnsignedValue(), _width);
    }

    SOLVER_EXPR_TYPE solverSignedExprFromGround(unsigned long x, size_t width) const {
      std::stringstream s;
      s << "0d" << width << "_" << (signed long)x;
      return msat_make_number(getSolver(), s.str().c_str()); 
    }


    SOLVER_EXPR_TYPE solverBooleanExprFromGround(const ExpressionImpGround* const e) const {
      return solverBooleanExprFromGround(e->getBooleanValue());
    }


    SOLVER_EXPR_TYPE solverBooleanExprFromGround(bool x) const {
      return x ? msat_make_true(getSolver()) : msat_make_false(getSolver());
    }

    SOLVER_EXPR_TYPE _expr;
    size_t _width;
    MathSATType _type;
  };

  class BVExpressionFactoryMathSAT : public ExpressionFactory {
  public:
    BVExpressionFactoryMathSAT() 
      : ExpressionFactory(new BVExpressionImpMathSAT()) {
    }
  };

} // namespace UrsaMajor

#endif
