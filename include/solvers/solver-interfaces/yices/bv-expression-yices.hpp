#if !defined __BV_EXPRESSION_YICES__
#define __BV_EXPRESSION_YICES__

#include <cassert>

#include "../expression.hpp"
#include "yices-instance.hpp"

namespace UrsaMajor {
  class BVExpressionImpYices : public ExpressionImpSymbolic {
  public:
    typedef yices_expr SOLVER_EXPR_TYPE;
    typedef yices_context SOLVER_TYPE;
    enum YicesType {YICES_BOOLEAN, YICES_BITVECTOR};

    explicit BVExpressionImpYices() 
      : ExpressionImpSymbolic(EK_ExpressionImpYices), _expr(0) {
    }

    static bool classof(const ExpressionImp *S) {
    	return S->getKind() == EK_ExpressionImpYices;
    }


    explicit BVExpressionImpYices(SOLVER_EXPR_TYPE expr, 
				  size_t width, YicesType type) 
      : ExpressionImpSymbolic(EK_ExpressionImpYices), _expr(expr), _width(width), _type(type) {
    }

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
      if (_type == YICES_BITVECTOR) {
	return YicesInstance::instance().getUnsignedAssignment(_expr, _width);
      } else if (_type == YICES_BOOLEAN) {
	return YicesInstance::instance().getBooleanAssignment(_expr);
      }
      return "error";
    }
    
    virtual ExpressionImp* unsignedSymbolic(const std::string& name, size_t width) const {
      yices_type type = yices_mk_bitvector_type(getSolver(), width);
      yices_var_decl decl = 
	yices_mk_var_decl(getSolver(), const_cast<char*>(name.c_str()), type);
      SOLVER_EXPR_TYPE exp = yices_mk_var_from_decl(getSolver(), decl);
      return new BVExpressionImpYices(exp, width, YICES_BITVECTOR);
    }

    virtual ExpressionImp* signedSymbolic(const std::string& name, size_t width) const {
      yices_type type = yices_mk_bitvector_type(getSolver(), width);
      yices_var_decl decl = 
        yices_mk_var_decl(getSolver(), const_cast<char*>(name.c_str()), type);
      SOLVER_EXPR_TYPE exp = yices_mk_var_from_decl(getSolver(), decl);
      return new BVExpressionImpYices(exp, width, YICES_BITVECTOR);
    }



    virtual ExpressionImp* integerGround(unsigned x, size_t width) const {
      SOLVER_EXPR_TYPE exp = yices_mk_bv_constant(getSolver(), width, x);
      return new BVExpressionImpYices(exp, width, YICES_BITVECTOR);
    }

    virtual ExpressionImp* integerGround(int x, size_t width) const {
      SOLVER_EXPR_TYPE exp = yices_mk_bv_constant(getSolver(), width, x);
      return new BVExpressionImpYices(exp, width, YICES_BITVECTOR);
    }


    virtual ExpressionImp* integerGround(unsigned long x, size_t width) const {
      SOLVER_EXPR_TYPE exp = yices_mk_bv_constant(getSolver(), width, x);
      return new BVExpressionImpYices(exp, width, YICES_BITVECTOR);
    }

    virtual ExpressionImp* integerGround(signed long x, size_t width) const {
      SOLVER_EXPR_TYPE exp = yices_mk_bv_constant(getSolver(), width, x);
      return new BVExpressionImpYices(exp, width, YICES_BITVECTOR);
    }

    virtual ExpressionImp* integerGround(unsigned short x, size_t width) const {
      SOLVER_EXPR_TYPE exp = yices_mk_bv_constant(getSolver(), width, x);
      return new BVExpressionImpYices(exp, width, YICES_BITVECTOR);
    }

    virtual ExpressionImp* integerGround(signed short x, size_t width) const {
      SOLVER_EXPR_TYPE exp = yices_mk_bv_constant(getSolver(), width, x);
      return new BVExpressionImpYices(exp, width, YICES_BITVECTOR);
    }


    virtual ExpressionImp* booleanSymbolic(const std::string& name) const {
      yices_type type = yices_mk_type(getSolver(), const_cast<char*>("bool"));
      yices_var_decl decl = 
	yices_mk_var_decl(getSolver(), const_cast<char*>(name.c_str()), type);
      SOLVER_EXPR_TYPE exp = yices_mk_bool_var_from_decl(getSolver(), decl);
      return new BVExpressionImpYices(exp, 1, YICES_BOOLEAN);
    }

    virtual ExpressionImp* booleanGround(bool x) const {
      SOLVER_EXPR_TYPE exp = x ? yices_mk_true(getSolver()) : 
	yices_mk_false(getSolver());
      return new BVExpressionImpYices(exp, 1, YICES_BOOLEAN);
    }



    yices_type translateType(Type t) ;

    virtual ExpressionImp* uninterpretedFunction(const Function& fun, const std::vector<const ExpressionImp*>& args) ;






    virtual ExpressionImp* addGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
	yices_mk_bv_add(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
      return new BVExpressionImpYices(exp, _width, YICES_BITVECTOR);
    }

    virtual ExpressionImp* addSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpYices* const bexpr = 
	llvm::dyn_cast<BVExpressionImpYices>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE exp = yices_mk_bv_add(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpYices(exp, _width, YICES_BITVECTOR);
    }

    virtual ExpressionImp* subtractGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
	yices_mk_bv_sub(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
      return new BVExpressionImpYices(exp, _width, YICES_BITVECTOR);
    }

    virtual ExpressionImp* subtractFromGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
	yices_mk_bv_sub(getSolver(), solverUnsignedExprFromGround(e), this->_expr);
      return new BVExpressionImpYices(exp, _width, YICES_BITVECTOR);
    }

    virtual ExpressionImp* subtractSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpYices* const bexpr = 
	llvm::dyn_cast<BVExpressionImpYices>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE exp = 
	yices_mk_bv_sub(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpYices(exp, _width, YICES_BITVECTOR);
    }

    virtual ExpressionImp* subtractFromSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpYices* const bexpr = 
	llvm::dyn_cast<BVExpressionImpYices>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE exp = 
	yices_mk_bv_sub(getSolver(), bexpr->_expr, this->_expr);
      return new BVExpressionImpYices(exp, _width, YICES_BITVECTOR);
    }

    virtual ExpressionImp* multGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
	yices_mk_bv_mul(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
      return new BVExpressionImpYices(exp, _width, YICES_BITVECTOR);
    }

    virtual ExpressionImp* multSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpYices* const bexpr = 
	llvm::dyn_cast<BVExpressionImpYices>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE exp = 
	yices_mk_bv_mul(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpYices(exp, _width, YICES_BITVECTOR);
    }



//FIXME Mozda umesto izuzetaka napraviti novu promenljivu ????
    virtual ExpressionImp* array(const std::string& name, size_t domain, size_t range) {
      throw "yices::Arrays not supported!";
    }

    virtual ExpressionImp* select(const ExpressionImp* const array, const ExpressionImp* const index) const {
      throw "yices::Arrays not supported!";
    }
    
    virtual ExpressionImp* store(const ExpressionImp* const array, const ExpressionImp* const index, const ExpressionImp* const value) const {
      throw "yices::Arrays not supported!";
    }

    virtual ExpressionImp* udivGround(const ExpressionImpGroundInteger* const e) const {
      throw "yices::ExpressionImpSymbolic::udivGround N/A"; 
   }

    virtual ExpressionImp* udivFromGround(const ExpressionImpGroundInteger* const e) const {
      throw "yices::ExpressionImpSymbolic::udivFromGround N/A"; 
    }

    virtual ExpressionImp* udivSymbolic(const ExpressionImpSymbolic* const e) const {
      throw "yices::ExpressionImpSymbolic::udivSymbolic N/A"; 
    }

    virtual ExpressionImp* udivFromSymbolic(const ExpressionImpSymbolic* const e) const {
      throw "yices::ExpressionImpSymbolic::udivFromSymbolic N/A"; 
    }

    virtual ExpressionImp* sdivGround(const ExpressionImpGroundInteger* const e) const {
      throw "yices::ExpressionImpSymbolic::sdivGround N/A"; 
   }

    virtual ExpressionImp* sdivFromGround(const ExpressionImpGroundInteger* const e) const {
      throw "yices::ExpressionImpSymbolic::sdivFromGround N/A"; 
    }

    virtual ExpressionImp* sdivSymbolic(const ExpressionImpSymbolic* const e) const {
      throw "yices::ExpressionImpSymbolic::sdivSymbolic N/A"; 
    }

    virtual ExpressionImp* sdivFromSymbolic(const ExpressionImpSymbolic* const e) const {
      throw "yices::ExpressionImpSymbolic::sdivFromSymbolic N/A"; 
    }

    virtual ExpressionImp* uremGround(const ExpressionImpGroundInteger* const e) const {
      throw "yices::ExpressionImpSymbolic::uremGround N/A"; 
   }

    virtual ExpressionImp* uremFromGround(const ExpressionImpGroundInteger* const e) const {
      throw "yices::ExpressionImpSymbolic::uremFromGround N/A"; 
    }

    virtual ExpressionImp* uremSymbolic(const ExpressionImpSymbolic* const e) const {
      throw "yices::ExpressionImpSymbolic::uremSymbolic N/A"; 
    }

    virtual ExpressionImp* uremFromSymbolic(const ExpressionImpSymbolic* const e) const {
      throw "yices::ExpressionImpSymbolic::uremFromSymbolic N/A"; 
    }


    virtual ExpressionImp* sremGround(const ExpressionImpGroundInteger* const e) const {
      throw "yices::ExpressionImpSymbolic::sremGround N/A"; 
   }

    virtual ExpressionImp* sremFromGround(const ExpressionImpGroundInteger* const e) const {
      throw "yices::ExpressionImpSymbolic::sremFromGround N/A"; 
    }

    virtual ExpressionImp* sremSymbolic(const ExpressionImpSymbolic* const e) const {
      throw "yices::ExpressionImpSymbolic::sremSymbolic N/A"; 
    }

    virtual ExpressionImp* sremFromSymbolic(const ExpressionImpSymbolic* const e) const {
      throw "yices::ExpressionImpSymbolic::sremFromSymbolic N/A"; 
    }




    virtual ExpressionImp* shiftLGround(const ExpressionImpGroundInteger* const e) const {
      throw "ExpressionImpSymbolic::shiftLGround N/A";
    }

    virtual ExpressionImp* shiftLForSymbolic(const ExpressionImpSymbolic* const e) const {
      throw "ExpressionImpSymbolic::shiftLForSymbolic N/A";
    }

    virtual ExpressionImp* shiftLSymbolic(const ExpressionImpSymbolic* const e) const {
      throw "ExpressionImpSymbolic::shiftLSymbolic N/A";
    }

    virtual ExpressionImp* aShiftRForGround(const ExpressionImpGroundInteger* const e) const {
      throw "ExpressionImpSymbolic::ashiftRForGround N/A";
    }

    virtual ExpressionImp* aShiftRGround(const ExpressionImpGroundInteger* const e) const {
      throw "ExpressionImpSymbolic::ashiftRGround N/A";
    }

    virtual ExpressionImp* aShiftRForSymbolic(const ExpressionImpSymbolic* const e) const {
      throw "ExpressionImpSymbolic::ashiftRForSymbolic N/A";
    }

    virtual ExpressionImp* aShiftRSymbolic(const ExpressionImpSymbolic* const e) const {
      throw "ExpressionImpSymbolic::ashiftRSymbolic N/A";
    }

    virtual ExpressionImp* lShiftRGround(const ExpressionImpGroundInteger* const e) const {
      throw "ExpressionImpSymbolic::lshiftRGround N/A";
    }

    virtual ExpressionImp* lShiftRForSymbolic(const ExpressionImpSymbolic* const e) const {
      throw "ExpressionImpSymbolic::lshiftRForSymbolic N/A";
    }

    virtual ExpressionImp* lShiftRSymbolic(const ExpressionImpSymbolic* const e) const {
      throw "ExpressionImpSymbolic::lshiftRSymbolic N/A";
    }






    virtual ExpressionImp* bitXorGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
	yices_mk_bv_xor(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
      return new BVExpressionImpYices(exp, _width, YICES_BITVECTOR);
    }

    virtual ExpressionImp* bitXorSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpYices* const bexpr = 
	llvm::dyn_cast<BVExpressionImpYices>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE exp = 
	yices_mk_bv_xor(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpYices(exp, _width, YICES_BITVECTOR);
    }

    virtual ExpressionImp* bitAndGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
	yices_mk_bv_and(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
      return new BVExpressionImpYices(exp, _width, YICES_BITVECTOR);
    }

    virtual ExpressionImp* bitAndSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpYices* const bexpr = 
	llvm::dyn_cast<BVExpressionImpYices>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE exp = 
	yices_mk_bv_and(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpYices(exp, _width, YICES_BITVECTOR);
    }

    virtual ExpressionImp* logicalAndGround(const ExpressionImpGroundBoolean* const e) const {
      SOLVER_EXPR_TYPE args[2];
      args[0] = this->_expr;
      args[1] = solverBooleanExprFromGround(e);
      SOLVER_EXPR_TYPE exp = 
	yices_mk_and(getSolver(), args, 2);
      return new BVExpressionImpYices(exp, _width, YICES_BOOLEAN);
    }

    virtual ExpressionImp* logicalAndSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpYices* const bexpr = 
	llvm::dyn_cast<BVExpressionImpYices>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE args[2];
      args[0] = this->_expr;
      args[1] = bexpr->_expr;
      SOLVER_EXPR_TYPE exp = 
	yices_mk_and(getSolver(), args, 2);
      return new BVExpressionImpYices(exp, _width, YICES_BOOLEAN);
    }

    virtual ExpressionImp* bitOrGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
	yices_mk_bv_or(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
      return new BVExpressionImpYices(exp, _width, YICES_BITVECTOR);
    }

    virtual ExpressionImp* bitOrSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpYices* const bexpr = 
	llvm::dyn_cast<BVExpressionImpYices>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE exp = 
	yices_mk_bv_or(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpYices(exp, _width, YICES_BITVECTOR);
    }

    virtual ExpressionImp* logicalOrGround(const ExpressionImpGroundBoolean* const e) const {
      SOLVER_EXPR_TYPE args[2];
      args[0] = this->_expr;
      args[1] = solverBooleanExprFromGround(e);
      SOLVER_EXPR_TYPE exp = 
	yices_mk_or(getSolver(), args, 2);
      return new BVExpressionImpYices(exp, _width, YICES_BOOLEAN);
    }

    virtual ExpressionImp* logicalOrSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpYices* const bexpr = 
	llvm::dyn_cast<BVExpressionImpYices>(e);
      assert(bexpr != 0);
      SOLVER_EXPR_TYPE args[2];
      args[0] = this->_expr;
      args[1] = bexpr->_expr;
      SOLVER_EXPR_TYPE exp = 
	yices_mk_or(getSolver(), args, 2);
      return new BVExpressionImpYices(exp, _width, YICES_BOOLEAN);
    }


    virtual ExpressionImp* bitNot() const {
      SOLVER_EXPR_TYPE exp = 
	yices_mk_bv_not(getSolver(), this->_expr);
      return new BVExpressionImpYices(exp, _width, YICES_BITVECTOR);
    }

    virtual ExpressionImp* logicalNot() const {
      SOLVER_EXPR_TYPE exp = 
	yices_mk_not(getSolver(), this->_expr);
      return new BVExpressionImpYices(exp, _width, YICES_BOOLEAN);
    }

    virtual ExpressionImp* shiftLForGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
	yices_mk_bv_shift_left0(getSolver(), this->_expr, e->getUnsignedValue());
      return new BVExpressionImpYices(exp, _width, YICES_BITVECTOR);
    }

    virtual ExpressionImp* lShiftRForGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
	yices_mk_bv_shift_right0(getSolver(), this->_expr, e->getUnsignedValue());
      return new BVExpressionImpYices(exp, _width, YICES_BITVECTOR);
    }

//    virtual ExpressionImp* aShiftRForGround(const ExpressionImpGroundInteger* const e) const {
//      throw "yices::ExpressionImpSymbolic::aShiftRForGround N/A"; 
//    }

    virtual ExpressionImp* sltGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
	yices_mk_bv_slt(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
      return new BVExpressionImpYices(exp, 1, YICES_BOOLEAN);
    }

    virtual ExpressionImp* sltSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpYices* const bexpr = 
	llvm::dyn_cast<BVExpressionImpYices>(e);
      assert(bexpr != 0);      
      SOLVER_EXPR_TYPE exp = 
	yices_mk_bv_slt(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpYices(exp, 1, YICES_BOOLEAN);
    }


    virtual ExpressionImp* sgtGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
	yices_mk_bv_sgt(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
      return new BVExpressionImpYices(exp, 1, YICES_BOOLEAN);
    }

    virtual ExpressionImp* sgtSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpYices* const bexpr = 
	llvm::dyn_cast<BVExpressionImpYices>(e);
      assert(bexpr != 0);      
      SOLVER_EXPR_TYPE exp = 
	yices_mk_bv_sgt(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpYices(exp, 1, YICES_BOOLEAN);
    }


    virtual ExpressionImp* sleGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
        yices_mk_bv_sle(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
      return new BVExpressionImpYices(exp, 1, YICES_BOOLEAN);
    }

    virtual ExpressionImp* sleSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpYices* const bexpr = 
        llvm::dyn_cast<BVExpressionImpYices>(e);
      assert(bexpr != 0);      
      SOLVER_EXPR_TYPE exp = 
        yices_mk_bv_sle(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpYices(exp, 1, YICES_BOOLEAN);
    }


    virtual ExpressionImp* sgeGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
        yices_mk_bv_sge(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
      return new BVExpressionImpYices(exp, 1, YICES_BOOLEAN);
    }

    virtual ExpressionImp* sgeSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpYices* const bexpr = 
        llvm::dyn_cast<BVExpressionImpYices>(e);
      assert(bexpr != 0);      
      SOLVER_EXPR_TYPE exp = 
        yices_mk_bv_sge(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpYices(exp, 1, YICES_BOOLEAN);
    }


    virtual ExpressionImp* ultGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
        yices_mk_bv_lt(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
      return new BVExpressionImpYices(exp, 1, YICES_BOOLEAN);
    }

    virtual ExpressionImp* ultSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpYices* const bexpr = 
        llvm::dyn_cast<BVExpressionImpYices>(e);
      assert(bexpr != 0);      
      SOLVER_EXPR_TYPE exp = 
        yices_mk_bv_lt(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpYices(exp, 1, YICES_BOOLEAN);
    }


    virtual ExpressionImp* ugtGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
        yices_mk_bv_gt(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
      return new BVExpressionImpYices(exp, 1, YICES_BOOLEAN);
    }

    virtual ExpressionImp* ugtSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpYices* const bexpr = 
        llvm::dyn_cast<BVExpressionImpYices>(e);
      assert(bexpr != 0);      
      SOLVER_EXPR_TYPE exp = 
        yices_mk_bv_gt(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpYices(exp, 1, YICES_BOOLEAN);
    }


    virtual ExpressionImp* uleGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
        yices_mk_bv_le(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
      return new BVExpressionImpYices(exp, 1, YICES_BOOLEAN);
    }

    virtual ExpressionImp* uleSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpYices* const bexpr = 
        llvm::dyn_cast<BVExpressionImpYices>(e);
      assert(bexpr != 0);      
      SOLVER_EXPR_TYPE exp = 
        yices_mk_bv_le(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpYices(exp, 1, YICES_BOOLEAN);
    }


    virtual ExpressionImp* ugeGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
        yices_mk_bv_ge(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
      return new BVExpressionImpYices(exp, 1, YICES_BOOLEAN);
    }

    virtual ExpressionImp* ugeSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpYices* const bexpr = 
        llvm::dyn_cast<BVExpressionImpYices>(e);
      assert(bexpr != 0);      
      SOLVER_EXPR_TYPE exp = 
        yices_mk_bv_ge(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpYices(exp, 1, YICES_BOOLEAN);
    }



//FIXME da li ovo moze da bude negativno width - _width 
    virtual ExpressionImp* zextimp(size_t width) const {
//      SOLVER_EXPR_TYPE exp = yices_mk_bv_sign_extend(getSolver(), this->_expr, width);
//      return new BVExpressionImpYices(exp, _width, YICES_BITVECTOR);
      throw "yices::ExpressionImpSymbolic::zextSymbolic N/A";
    }

//FIXME da li ovo moze da bude negativno width - _width 
    virtual ExpressionImp* sextimp(size_t width) const {
      SOLVER_EXPR_TYPE exp = yices_mk_bv_sign_extend(getSolver(), this->_expr, width - _width );
      return new BVExpressionImpYices(exp, width, YICES_BITVECTOR);
    }


//FIXME ovo mozda postoji, proveriti
    virtual ExpressionImp* extractimp(size_t low, size_t high) const {
      throw "yices::ExpressionImpSymbolic::extractSymbolic N/A";
    }

    virtual ExpressionImp* eqGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
	yices_mk_eq(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
      return new BVExpressionImpYices(exp, 1, YICES_BOOLEAN);
    }

    virtual ExpressionImp* eqGround(const ExpressionImpGroundBoolean* const e) const {
      SOLVER_EXPR_TYPE exp = 
	yices_mk_eq(getSolver(), this->_expr, solverBooleanExprFromGround(e));
      return new BVExpressionImpYices(exp, 1, YICES_BOOLEAN);
    }

    virtual ExpressionImp* eqSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpYices* const bexpr = 
	llvm::dyn_cast<BVExpressionImpYices>(e);
      assert(bexpr != 0);      
      SOLVER_EXPR_TYPE exp = 
	yices_mk_eq(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpYices(exp, 1, YICES_BOOLEAN);
    }

    virtual ExpressionImp* neqGround(const ExpressionImpGroundInteger* const e) const {
      SOLVER_EXPR_TYPE exp = 
	yices_mk_diseq(getSolver(), this->_expr, solverUnsignedExprFromGround(e));
      return new BVExpressionImpYices(exp, 1, YICES_BOOLEAN);
    }

    virtual ExpressionImp* neqGround(const ExpressionImpGroundBoolean* const e) const {
      SOLVER_EXPR_TYPE exp = 
	yices_mk_diseq(getSolver(), this->_expr, solverBooleanExprFromGround(e));
      return new BVExpressionImpYices(exp, 1, YICES_BOOLEAN);
    }

    virtual ExpressionImp* neqSymbolic(const ExpressionImpSymbolic* const e) const {
      const BVExpressionImpYices* const bexpr = 
	llvm::dyn_cast<BVExpressionImpYices>(e);
      assert(bexpr != 0);      
      SOLVER_EXPR_TYPE exp = 
	yices_mk_diseq(getSolver(), this->_expr, bexpr->_expr);
      return new BVExpressionImpYices(exp, 1, YICES_BOOLEAN);
    }

    virtual ExpressionImp* ite_(ExpressionImp* thenExpr,
			    ExpressionImp* elseExpr) const {
      BVExpressionImpYices* bthenExpr = 
	llvm::dyn_cast<BVExpressionImpYices>(thenExpr);
      BVExpressionImpYices*  belseExpr =
	llvm::dyn_cast<BVExpressionImpYices>(elseExpr);
      assert(bthenExpr != 0 && belseExpr != 0);
      SOLVER_EXPR_TYPE exp = 
	yices_mk_ite(getSolver(), this->_expr, bthenExpr->_expr, belseExpr->_expr);
      return new BVExpressionImpYices(exp, bthenExpr->_width, YICES_BITVECTOR);
    }


    virtual void print(std::ostream& ostr) const {
      yices_pp_expr(_expr);
    }


  private:
    static SOLVER_TYPE getSolver() {
      return YicesInstance::instance().getSolver();
    }

    SOLVER_EXPR_TYPE solverUnsignedExprFromGround(const ExpressionImpGroundInteger* const e) const ;

    SOLVER_EXPR_TYPE solverBooleanExprFromGround(const ExpressionImpGround* const e) const {
      unsigned value = e->getBooleanValue();
      return value ? yices_mk_true(getSolver()) : yices_mk_false(getSolver());
    }

    SOLVER_EXPR_TYPE _expr;
    size_t _width;
    YicesType _type;
  };

  class BVExpressionFactoryYices : public ExpressionFactory {
  public:
    BVExpressionFactoryYices() 
      : ExpressionFactory(new BVExpressionImpYices()) {
    }
  };

 
} // namespace UrsaMajor

#endif
