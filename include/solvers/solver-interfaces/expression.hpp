#if !defined __BV_EXPRESSION__
#define __BV_EXPRESSION__

#include <string>
#include <iostream>
#include <map>
#include <tr1/memory>

#include "../syntax/function.hpp"
#include "bit-vector.hpp"

#include "llvm/Support/Casting.h"

namespace UrsaMajor {
class ExpressionFactory;
class ExpressionImp;

// ---------------------------------------------------------------------------
class Expression {
public:
  static void setFactory(ExpressionFactory *factory);

  explicit Expression(ExpressionImp *imp = 0);
  explicit Expression(std::tr1::shared_ptr<ExpressionImp> imp);
  ~Expression() {}

  static Expression integerGround(unsigned int x, size_t width);
  static Expression integerGround(signed int x, size_t width);
  static Expression integerGround(unsigned long x, size_t width);
  static Expression integerGround(signed long x, size_t width);
  static Expression integerGround(unsigned short x, size_t width);
  static Expression integerGround(signed short x, size_t width);

  static Expression unsignedSymbolic(const std::string &name, size_t width);
  static Expression signedSymbolic(const std::string &name, size_t width);
  static Expression booleanGround(bool x);
  static Expression booleanSymbolic(const std::string &name);

  static Expression uninterpretedFunction(const Function &f,
                                          const std::vector<Expression> &args);
  static Expression array(const std::string &name, size_t domain, size_t range);

  static Expression select(const Expression &array, const Expression &index);
  static Expression store(const Expression &array, const Expression &index,
                          const Expression &value);

  bool checkSat();
  bool addConstraint();
  bool addTempConstraint();
  void reset();
  std::string getAssignment() const;

  Expression operator+(const Expression &expr) const;
  Expression operator-(const Expression &expr) const;
  Expression operator*(const Expression &expr) const;
  Expression operator^(const Expression &expr) const;
  Expression operator&(const Expression &expr) const;
  Expression operator<<(const Expression &expr) const;
  //    Expression operator>> (const Expression& expr) const;
  Expression operator&&(const Expression &expr) const;
  Expression operator|(const Expression &expr) const;
  Expression operator||(const Expression &expr) const;
  Expression operator~() const;
  Expression operator!() const;
  //    Expression operator>  (const Expression& expr) const;
  //    Expression operator<  (const Expression& expr) const;
  Expression operator==(const Expression &expr) const;
  Expression operator!=(const Expression &expr) const;
  Expression ite(const Expression &thenExpr, const Expression &elseExpr,
                 size_t width) const;

  Expression aShiftR(const Expression &expr) const;
  Expression lShiftR(const Expression &expr) const;

  Expression udiv(const Expression &expr) const;
  Expression sdiv(const Expression &expr) const;
  Expression urem(const Expression &expr) const;
  Expression srem(const Expression &expr) const;

  Expression ult(const Expression &expr) const;
  Expression slt(const Expression &expr) const;
  Expression ugt(const Expression &expr) const;
  Expression sgt(const Expression &expr) const;

  Expression ule(const Expression &expr) const;
  Expression sle(const Expression &expr) const;
  Expression uge(const Expression &expr) const;
  Expression sge(const Expression &expr) const;

  Expression zext(size_t width) const;
  Expression sext(size_t width) const;
  Expression extract(size_t low, size_t high) const;

  void print(std::ostream &ostr) const;

  unsigned long getUnsignedValue() const;
  bool getBooleanValue() const;

  ExpressionImp *getImp() const { return _imp.get(); }
  //    static std::map<std::string, std::tr1::shared_ptr<ExpressionImp> >
  // _symbolTable;
private:
  void setImp(ExpressionImp *imp) {
    _imp = std::tr1::shared_ptr<ExpressionImp>(imp);
  }

  std::tr1::shared_ptr<ExpressionImp> _imp;
  thread_local static ExpressionFactory *_factory;
  friend class ExpressionFactory;

};

std::ostream &operator<<(std::ostream &ostr, const Expression &e);

class ExpressionImpGround;
class ExpressionImpGroundInteger;
class ExpressionImpGroundBoolean;
class ExpressionImpSymbolic;

// ---------------------------------------------------------------------------
class ExpressionImp {
public:
  virtual ~ExpressionImp() {}

  enum ExpressionImpKind {
    EK_ExpressionImpGround,
    EK_ExpressionImpGroundBoolean,
    EK_ExpressionImpGroundInteger,
    EK_ExpressionImpSymbolic,
    EK_ExpressionImpBoolector,
    EK_ExpressionImpZ3,
    EK_ExpressionImpYices,
    EK_ExpressionImpMathSAT
  };

protected:
  const ExpressionImpKind Kind;

public:
  ExpressionImpKind getKind() const { return Kind; }
  ExpressionImp(ExpressionImpKind k) : Kind(k) {}

  virtual bool checkSat() = 0;
  virtual bool addTempConstraint() = 0;
  virtual bool addConstraint() = 0;
  virtual void reset() = 0;
  virtual std::string getAssignment() const = 0;

  virtual ExpressionImp *signedSymbolic(const std::string &name,
                                        size_t width) const = 0;
  virtual ExpressionImp *unsignedSymbolic(const std::string &name,
                                          size_t width) const = 0;
  virtual ExpressionImp *integerGround(unsigned int x, size_t width) const = 0;
  virtual ExpressionImp *integerGround(signed int x, size_t width) const = 0;
  virtual ExpressionImp *integerGround(unsigned long x, size_t width) const = 0;
  virtual ExpressionImp *integerGround(signed long x, size_t width) const = 0;
  virtual ExpressionImp *integerGround(unsigned short x,
                                       size_t width) const = 0;
  virtual ExpressionImp *integerGround(signed short x, size_t width) const = 0;
  virtual ExpressionImp *booleanSymbolic(const std::string &name) const = 0;
  virtual ExpressionImp *booleanGround(bool x) const = 0;

  virtual ExpressionImp *add(const ExpressionImp *const e) const = 0;
  virtual ExpressionImp *
      addGround(const ExpressionImpGroundInteger *const e) const = 0;
  virtual ExpressionImp *
      addSymbolic(const ExpressionImpSymbolic *const e) const = 0;

  virtual ExpressionImp *subtract(const ExpressionImp *const e) const = 0;
  virtual ExpressionImp *
      subtractGround(const ExpressionImpGroundInteger *const e) const = 0;
  virtual ExpressionImp *
      subtractSymbolic(const ExpressionImpSymbolic *const e) const = 0;
  virtual ExpressionImp *
      subtractFromGround(const ExpressionImpGroundInteger *const e) const = 0;
  virtual ExpressionImp *
      subtractFromSymbolic(const ExpressionImpSymbolic *const e) const = 0;

  virtual ExpressionImp *mult(const ExpressionImp *const e) const = 0;
  virtual ExpressionImp *
      multGround(const ExpressionImpGroundInteger *const e) const = 0;
  virtual ExpressionImp *
      multSymbolic(const ExpressionImpSymbolic *const e) const = 0;

  virtual ExpressionImp *udivimp(const ExpressionImp *const e) const = 0;
  virtual ExpressionImp *
      udivGround(const ExpressionImpGroundInteger *const e) const = 0;
  virtual ExpressionImp *
      udivSymbolic(const ExpressionImpSymbolic *const e) const = 0;
  virtual ExpressionImp *
      udivFromGround(const ExpressionImpGroundInteger *const e) const = 0;
  virtual ExpressionImp *
      udivFromSymbolic(const ExpressionImpSymbolic *const e) const = 0;

  virtual ExpressionImp *sdivimp(const ExpressionImp *const e) const = 0;
  virtual ExpressionImp *
      sdivGround(const ExpressionImpGroundInteger *const e) const = 0;
  virtual ExpressionImp *
      sdivSymbolic(const ExpressionImpSymbolic *const e) const = 0;
  virtual ExpressionImp *
      sdivFromGround(const ExpressionImpGroundInteger *const e) const = 0;
  virtual ExpressionImp *
      sdivFromSymbolic(const ExpressionImpSymbolic *const e) const = 0;

  virtual ExpressionImp *uremimp(const ExpressionImp *const e) const = 0;
  virtual ExpressionImp *
      uremGround(const ExpressionImpGroundInteger *const e) const = 0;
  virtual ExpressionImp *
      uremSymbolic(const ExpressionImpSymbolic *const e) const = 0;
  virtual ExpressionImp *
      uremFromGround(const ExpressionImpGroundInteger *const e) const = 0;
  virtual ExpressionImp *
      uremFromSymbolic(const ExpressionImpSymbolic *const e) const = 0;

  virtual ExpressionImp *sremimp(const ExpressionImp *const e) const = 0;
  virtual ExpressionImp *
      sremGround(const ExpressionImpGroundInteger *const e) const = 0;
  virtual ExpressionImp *
      sremSymbolic(const ExpressionImpSymbolic *const e) const = 0;
  virtual ExpressionImp *
      sremFromGround(const ExpressionImpGroundInteger *const e) const = 0;
  virtual ExpressionImp *
      sremFromSymbolic(const ExpressionImpSymbolic *const e) const = 0;

  virtual ExpressionImp *bitXor(const ExpressionImp *const e) const = 0;
  virtual ExpressionImp *
      bitXorGround(const ExpressionImpGroundInteger *const e) const = 0;
  virtual ExpressionImp *
      bitXorSymbolic(const ExpressionImpSymbolic *const e) const = 0;

  virtual ExpressionImp *bitAnd(const ExpressionImp *const e) const = 0;
  virtual ExpressionImp *
      bitAndGround(const ExpressionImpGroundInteger *const e) const = 0;
  virtual ExpressionImp *
      bitAndSymbolic(const ExpressionImpSymbolic *const e) const = 0;

  virtual ExpressionImp *logicalAnd(const ExpressionImp *const e) const = 0;
  virtual ExpressionImp *
      logicalAndGround(const ExpressionImpGroundBoolean *const e) const = 0;
  virtual ExpressionImp *
      logicalAndSymbolic(const ExpressionImpSymbolic *const e) const = 0;

  virtual ExpressionImp *bitOr(const ExpressionImp *const e) const = 0;
  virtual ExpressionImp *
      bitOrGround(const ExpressionImpGroundInteger *const e) const = 0;
  virtual ExpressionImp *
      bitOrSymbolic(const ExpressionImpSymbolic *const e) const = 0;

  virtual ExpressionImp *logicalOr(const ExpressionImp *const e) const = 0;
  virtual ExpressionImp *
      logicalOrGround(const ExpressionImpGroundBoolean *const e) const = 0;
  virtual ExpressionImp *
      logicalOrSymbolic(const ExpressionImpSymbolic *const e) const = 0;

  virtual ExpressionImp *bitNot() const = 0;
  virtual ExpressionImp *logicalNot() const = 0;

  virtual ExpressionImp *shiftL(const ExpressionImp *const e) const = 0;
  virtual ExpressionImp *
      shiftLForGround(const ExpressionImpGroundInteger *const e) const = 0;
  virtual ExpressionImp *
      shiftLForSymbolic(const ExpressionImpSymbolic *const e) const = 0;
  virtual ExpressionImp *
      shiftLGround(const ExpressionImpGroundInteger *const e) const = 0;
  virtual ExpressionImp *
      shiftLSymbolic(const ExpressionImpSymbolic *const e) const = 0;

  virtual ExpressionImp *aShiftRimp(const ExpressionImp *const e) const = 0;
  virtual ExpressionImp *
      aShiftRForGround(const ExpressionImpGroundInteger *const e) const = 0;
  virtual ExpressionImp *
      aShiftRForSymbolic(const ExpressionImpSymbolic *const e) const = 0;
  virtual ExpressionImp *
      aShiftRGround(const ExpressionImpGroundInteger *const e) const = 0;
  virtual ExpressionImp *
      aShiftRSymbolic(const ExpressionImpSymbolic *const e) const = 0;

  virtual ExpressionImp *lShiftRimp(const ExpressionImp *const e) const = 0;
  virtual ExpressionImp *
      lShiftRForGround(const ExpressionImpGroundInteger *const e) const = 0;
  virtual ExpressionImp *
      lShiftRForSymbolic(const ExpressionImpSymbolic *const e) const = 0;
  virtual ExpressionImp *
      lShiftRGround(const ExpressionImpGroundInteger *const e) const = 0;
  virtual ExpressionImp *
      lShiftRSymbolic(const ExpressionImpSymbolic *const e) const = 0;

  virtual ExpressionImp *ultimp(const ExpressionImp *const e) const = 0;
  virtual ExpressionImp *
      ultGround(const ExpressionImpGroundInteger *const e) const = 0;
  virtual ExpressionImp *
      ultSymbolic(const ExpressionImpSymbolic *const e) const = 0;

  virtual ExpressionImp *ugtimp(const ExpressionImp *const e) const = 0;
  virtual ExpressionImp *
      ugtGround(const ExpressionImpGroundInteger *const e) const = 0;
  virtual ExpressionImp *
      ugtSymbolic(const ExpressionImpSymbolic *const e) const = 0;

  virtual ExpressionImp *uleimp(const ExpressionImp *const e) const = 0;
  virtual ExpressionImp *
      uleGround(const ExpressionImpGroundInteger *const e) const = 0;
  virtual ExpressionImp *
      uleSymbolic(const ExpressionImpSymbolic *const e) const = 0;

  virtual ExpressionImp *ugeimp(const ExpressionImp *const e) const = 0;
  virtual ExpressionImp *
      ugeGround(const ExpressionImpGroundInteger *const e) const = 0;
  virtual ExpressionImp *
      ugeSymbolic(const ExpressionImpSymbolic *const e) const = 0;

  virtual ExpressionImp *sltimp(const ExpressionImp *const e) const = 0;
  virtual ExpressionImp *
      sltGround(const ExpressionImpGroundInteger *const e) const = 0;
  virtual ExpressionImp *
      sltSymbolic(const ExpressionImpSymbolic *const e) const = 0;

  virtual ExpressionImp *sgtimp(const ExpressionImp *const e) const = 0;
  virtual ExpressionImp *
      sgtGround(const ExpressionImpGroundInteger *const e) const = 0;
  virtual ExpressionImp *
      sgtSymbolic(const ExpressionImpSymbolic *const e) const = 0;

  virtual ExpressionImp *sleimp(const ExpressionImp *const e) const = 0;
  virtual ExpressionImp *
      sleGround(const ExpressionImpGroundInteger *const e) const = 0;
  virtual ExpressionImp *
      sleSymbolic(const ExpressionImpSymbolic *const e) const = 0;

  virtual ExpressionImp *sgeimp(const ExpressionImp *const e) const = 0;
  virtual ExpressionImp *
      sgeGround(const ExpressionImpGroundInteger *const e) const = 0;
  virtual ExpressionImp *
      sgeSymbolic(const ExpressionImpSymbolic *const e) const = 0;

  virtual ExpressionImp *eq(const ExpressionImp *const e) const = 0;
  virtual ExpressionImp *
      eqGround(const ExpressionImpGroundInteger *const e) const = 0;
  virtual ExpressionImp *
      eqGround(const ExpressionImpGroundBoolean *const e) const = 0;
  virtual ExpressionImp *
      eqSymbolic(const ExpressionImpSymbolic *const e) const = 0;

  virtual ExpressionImp *neq(const ExpressionImp *const e) const = 0;
  virtual ExpressionImp *
      neqGround(const ExpressionImpGroundInteger *const e) const = 0;
  virtual ExpressionImp *
      neqGround(const ExpressionImpGroundBoolean *const e) const = 0;
  virtual ExpressionImp *
      neqSymbolic(const ExpressionImpSymbolic *const e) const = 0;

  virtual ExpressionImp *zextimp(size_t width) const = 0;
  virtual ExpressionImp *sextimp(size_t width) const = 0;
  virtual ExpressionImp *extractimp(size_t low, size_t high) const = 0;

  virtual ExpressionImp *
      uninterpretedFunction(const Function &fun,
                            const std::vector<const ExpressionImp *> &args) = 0;
  virtual ExpressionImp *array(const std::string &name, size_t domain,
                               size_t range) = 0;

  virtual ExpressionImp *select(const ExpressionImp *const array,
                                const ExpressionImp *const index) const = 0;
  virtual ExpressionImp *store(const ExpressionImp *const array,
                               const ExpressionImp *const index,
                               const ExpressionImp *const value) const = 0;

  // TODO: FIXME const correctness?
  virtual ExpressionImp *ite(ExpressionImp *thenExpr, ExpressionImp *elseExpr,
                             size_t width) const = 0;

  virtual void print(std::ostream &ostr) const = 0;

  virtual bool isGround() const = 0;
  virtual bool isGroundBoolean() const { return false; }
  virtual bool isGroundInteger() const { return false; }
  virtual ExpressionImpGround *getGround() const = 0;
  virtual unsigned long getUnsignedValue() const = 0;
  virtual bool getBooleanValue() const = 0;
};

// ---------------------------------------------------------------------------
class ExpressionImpGround : public ExpressionImp {
public:
  virtual bool isGround() const { return true; }

  ExpressionImpGround() : ExpressionImp(EK_ExpressionImpGround) {}
  ExpressionImpGround(ExpressionImpKind k) : ExpressionImp(k) {}

  virtual ExpressionImpGround *getGround() const {
    // FIXME copy
    return 0;
  }

  static bool classof(const ExpressionImp *S) {
    return (S->getKind() == EK_ExpressionImpGroundBoolean ||
            S->getKind() == EK_ExpressionImpGround ||
            S->getKind() == EK_ExpressionImpGroundInteger);
  }

  virtual bool checkSat() { throw "ExpressionImpGround::checkSat() N/A"; }

  virtual bool addConstraint() {
    throw "ExpressionImpGround::addConstraint() N/A";
  }

  virtual bool addTempConstraint() {
    throw "ExpressionImpGround::addTempConstraint() N/A";
  }

  virtual void reset() { throw "ExpressionImpGround::reset() N/A"; }

  virtual std::string getAssignment() const {
    throw "ExpressionImpGround::getAssignment() N/A";
  }

  virtual ExpressionImp *integerGround(unsigned int n, size_t width) const {
    throw "ExpressionImpGround::integerGround N/A";
  }

  virtual ExpressionImp *integerGround(signed int n, size_t width) const {
    throw "ExpressionImpGround::integerGround N/A";
  }

  virtual ExpressionImp *integerGround(unsigned long n, size_t width) const {
    throw "ExpressionImpGround::integerGround N/A";
  }

  virtual ExpressionImp *integerGround(signed long n, size_t width) const {
    throw "ExpressionImpGround::integerGround N/A";
  }

  virtual ExpressionImp *integerGround(unsigned short n, size_t width) const {
    throw "ExpressionImpGround::integerGround N/A";
  }

  virtual ExpressionImp *integerGround(signed short n, size_t width) const {
    throw "ExpressionImpGround::integerGround N/A";
  }

  virtual ExpressionImp *unsignedSymbolic(const std::string &name,
                                          size_t width) const {
    throw "ExpressionImpGround::unsignedSymbolic N/A";
  }

  virtual ExpressionImp *signedSymbolic(const std::string &name,
                                        size_t width) const {
    throw "ExpressionImpGround::signedSymbolic N/A";
  }

  virtual ExpressionImp *booleanSymbolic(const std::string &name) const {
    throw "ExpressionImpGround::booleanSymbolic N/A";
  }

  virtual ExpressionImp *booleanGround(bool x) const {
    throw "ExpressionImpGround::booleanGround N/A";
  }

  virtual ExpressionImp *logicalAnd(const ExpressionImp *const e) const {
    throw "ExpressionImpGround::logicalAnd N/A";
  }

  virtual ExpressionImp *
  logicalAndGround(const ExpressionImpGroundBoolean *const e) const {
    throw "ExpressionImpGround::logicalAndGround N/A";
  }

  virtual ExpressionImp *
  logicalAndSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpGround::logicalAndGroundSymbolic N/A";
  }

  virtual ExpressionImp *logicalOr(const ExpressionImp *const e) const {
    throw "ExpressionImpGround::logicalOR N/A";
  }

  virtual ExpressionImp *
  logicalOrGround(const ExpressionImpGroundBoolean *const e) const {
    throw "ExpressionImpGround::logicalORGround N/A";
  }

  virtual ExpressionImp *
  logicalOrSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpGround::logicalORSymbolic N/A";
  }

  virtual ExpressionImp *logicalNot() const {
    throw "ExpressionImpGround::logicalNot N/A";
  }

  virtual ExpressionImp *
  eqGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpGround::eqGround N/A";
  }

  virtual ExpressionImp *
  eqGround(const ExpressionImpGroundBoolean *const e) const {
    throw "ExpressionImpGround::eqGround N/A 1";
  }

  virtual ExpressionImp *
  neqGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpGround::neqGround N/A";
  }

  virtual ExpressionImp *
  neqGround(const ExpressionImpGroundBoolean *const e) const {
    throw "ExpressionImpGround::neqGround N/A";
  }

  virtual ExpressionImp *add(const ExpressionImp *const e) const {
    throw "ExpressionImpGround::add N/A";
  }

  virtual ExpressionImp *
  addGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpGround::addGround N/A";
  }

  virtual ExpressionImp *
  addSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpGround::addSymbolic N/A";
  }

  virtual ExpressionImp *subtract(const ExpressionImp *const e) const {
    throw "ExpressionImpGround::subtract N/A";
  }

  virtual ExpressionImp *
  subtractGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpGround::subtractGround N/A";
  }

  virtual ExpressionImp *
  subtractFromGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpGround::subtractFromGround N/A";
  }

  virtual ExpressionImp *
  subtractSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpGround:subtractSymbolic N/A";
  }

  virtual ExpressionImp *
  subtractFromSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpGround::subtractSymbolic N/A";
  }

  virtual ExpressionImp *mult(const ExpressionImp *const e) const {
    throw "ExpressionImpGround::mult N/A";
  }

  virtual ExpressionImp *
  multGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpGround::multGround N/A";
  }

  virtual ExpressionImp *
  multSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpGround::multSymbolic N/A";
  }

  virtual ExpressionImp *udivimp(const ExpressionImp *const e) const {
    throw "ExpressionImpGround::udivimp N/A";
  }

  virtual ExpressionImp *
  udivGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpGround::udivGround N/A";
  }

  virtual ExpressionImp *
  udivSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpGround::udivSymbolic N/A";
  }

  virtual ExpressionImp *
  udivFromGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpGround::udivFromGround N/A";
  }

  virtual ExpressionImp *
  udivFromSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpGround::udivFromSymbolic N/A";
  }

  virtual ExpressionImp *sdivimp(const ExpressionImp *const e) const {
    throw "ExpressionImpGround::sdivimp N/A";
  }

  virtual ExpressionImp *
  sdivGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpGround::sdivGround N/A";
  }

  virtual ExpressionImp *
  sdivSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpGround::sdivSymbolic N/A";
  }

  virtual ExpressionImp *
  sdivFromGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpGround::sdivFromGround N/A";
  }

  virtual ExpressionImp *
  sdivFromSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpGround::sdivFromSymbolic N/A";
  }

  virtual ExpressionImp *uremimp(const ExpressionImp *const e) const {
    throw "ExpressionImpGround::uremimp N/A";
  }

  virtual ExpressionImp *
  uremGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpGround::uremGround N/A";
  }

  virtual ExpressionImp *
  uremSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpGround::uremSymbolic N/A";
  }

  virtual ExpressionImp *
  uremFromGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpGround::uremFromGround N/A";
  }

  virtual ExpressionImp *
  uremFromSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpGround::uremFromSymbolic N/A";
  }

  virtual ExpressionImp *sremimp(const ExpressionImp *const e) const {
    throw "ExpressionImpGround::uremimp N/A";
  }

  virtual ExpressionImp *
  sremGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpGround::uremGround N/A";
  }

  virtual ExpressionImp *
  sremSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpGround::uremSymbolic N/A";
  }

  virtual ExpressionImp *
  sremFromGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpGround::sremFromGround N/A";
  }

  virtual ExpressionImp *
  sremFromSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpGround::sremFromSymbolic N/A";
  }

  virtual ExpressionImp *bitXor(const ExpressionImp *const e) const {
    throw "ExpressionImpGround::bitXor N/A";
  }

  virtual ExpressionImp *
  bitXorGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpGround::bitXorGround N/A";
  }

  virtual ExpressionImp *
  bitXorSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpGround::bitXorSymbolic N/A";
  }

  virtual ExpressionImp *bitAnd(const ExpressionImp *const e) const {
    throw "ExpressionImpGround::bitAnd N/A";
  }

  virtual ExpressionImp *
  bitAndGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpGround::bitAndGround N/A";
  }

  virtual ExpressionImp *
  bitAndSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpGround::bitAndSymbolic N/A";
  }

  virtual ExpressionImp *bitOr(const ExpressionImp *const e) const {
    throw "ExpressionImpGround::bitOr N/A";
  }

  virtual ExpressionImp *
  bitOrGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpGround::bitOrGround N/A";
  }

  virtual ExpressionImp *
  bitOrSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpGround::bitOrSymbolic N/A";
  }

  virtual ExpressionImp *bitNot() const {
    throw "ExpressionImpGround::bitNot N/A";
  }

  virtual ExpressionImp *shiftL(const ExpressionImp *const e) const {
    throw "ExpressionImpGround::shiftL N/A";
  }

  virtual ExpressionImp *
  shiftLForGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpGround::shiftLForGround N/A";
  }

  virtual ExpressionImp *
  shiftLGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpGround::shiftLGround N/A";
  }

  virtual ExpressionImp *
  shiftLForSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpGround::shiftLForSymbolic N/A";
  }

  virtual ExpressionImp *
  shiftLSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpGround::shiftLSymbolic N/A";
  }

  virtual ExpressionImp *aShiftRimp(const ExpressionImp *const e) const {
    throw "ExpressionImpGround::shiftR N/A";
  }

  virtual ExpressionImp *
  aShiftRForGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpGround::shiftRForGround N/A";
  }

  virtual ExpressionImp *
  aShiftRGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpGround::shiftRGround N/A";
  }

  virtual ExpressionImp *
  aShiftRForSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpGround::shiftRForSymbolic N/A";
  }

  virtual ExpressionImp *
  aShiftRSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpGround::shiftRSymbolic N/A";
  }

  virtual ExpressionImp *lShiftRimp(const ExpressionImp *const e) const {
    throw "ExpressionImpGround::shiftR N/A";
  }

  virtual ExpressionImp *
  lShiftRForGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpGround::shiftRForGround N/A";
  }

  virtual ExpressionImp *
  lShiftRGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpGround::shiftRGround N/A";
  }

  virtual ExpressionImp *
  lShiftRForSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpGround::shiftRForSymbolic N/A";
  }

  virtual ExpressionImp *
  lShiftRSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpGround::shiftRSymbolic N/A";
  }

  virtual ExpressionImp *ultimp(const ExpressionImp *const e) const {
    throw "ExpressionImpGround::lt N/A";
  }

  virtual ExpressionImp *
  ultGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpGround::ltGround N/A";
  }

  virtual ExpressionImp *
  ultSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpGround::ltSymbolic N/A";
  }

  virtual ExpressionImp *ugtimp(const ExpressionImp *const e) const {
    throw "ExpressionImpGround::gt N/A";
  }

  virtual ExpressionImp *
  ugtGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpGround::gtGround N/A";
  }

  virtual ExpressionImp *
  ugtSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpGround::gtSymbolic N/A";
  }

  virtual ExpressionImp *uleimp(const ExpressionImp *const e) const {
    throw "ExpressionImpGround::ule N/A";
  }

  virtual ExpressionImp *
  uleGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpGround::uleGround N/A";
  }

  virtual ExpressionImp *
  uleSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpGround::uleSymbolic N/A";
  }

  virtual ExpressionImp *ugeimp(const ExpressionImp *const e) const {
    throw "ExpressionImpGround::uge N/A";
  }

  virtual ExpressionImp *
  ugeGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpGround::ugeGround N/A";
  }

  virtual ExpressionImp *
  ugeSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpGround::ugeSymbolic N/A";
  }

  virtual ExpressionImp *sltimp(const ExpressionImp *const e) const {
    throw "ExpressionImpGround::slt N/A";
  }

  virtual ExpressionImp *
  sltGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpGround::sltGround N/A";
  }

  virtual ExpressionImp *
  sltSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpGround::sltSymbolic N/A";
  }

  virtual ExpressionImp *sgtimp(const ExpressionImp *const e) const {
    throw "ExpressionImpGround::sgt N/A";
  }

  virtual ExpressionImp *
  sgtGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpGround::sgtGround N/A";
  }

  virtual ExpressionImp *
  sgtSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpGround::sgtSymbolic N/A";
  }

  virtual ExpressionImp *sleimp(const ExpressionImp *const e) const {
    throw "ExpressionImpGround::sle N/A";
  }

  virtual ExpressionImp *
  sleGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpGround::sleGround N/A";
  }

  virtual ExpressionImp *
  sleSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpGround::sleSymbolic N/A";
  }

  virtual ExpressionImp *sgeimp(const ExpressionImp *const e) const {
    throw "ExpressionImpGround::sge N/A";
  }

  virtual ExpressionImp *
  sgeGround(const ExpressionImpGroundInteger *const e) const {
    throw "ExpressionImpGround::sgeGround N/A";
  }

  virtual ExpressionImp *
  sgeSymbolic(const ExpressionImpSymbolic *const e) const {
    throw "ExpressionImpGround::sgeSymbolic N/A";
  }

  virtual ExpressionImp *ite(ExpressionImp *thenExpr, ExpressionImp *elseExpr,
                             size_t width) const {
    throw "ExpressionImpGround::ite N/A";
  }

  virtual ExpressionImp *zextimp(size_t width) const {
    throw "ExpressionImpGround::zextimp N/A";
  }

  virtual ExpressionImp *sextimp(size_t width) const {
    throw "ExpressionImpGround::sextimp N/A";
  }

  virtual ExpressionImp *extractimp(size_t low, size_t high) const {
    throw "ExpressionImpGround::extractimp N/A";
  }

  virtual ExpressionImp *
  uninterpretedFunction(const Function &fun,
                        const std::vector<const ExpressionImp *> &args) {
    throw "ExpressionImpGround::uninterpretedFunction N/A";
  }
  virtual ExpressionImp *array(const std::string &name, size_t domain,
                               size_t range) {
    throw "ExpressionImpGround::array N/A";
  }

  virtual ExpressionImp *select(const ExpressionImp *const array,
                                const ExpressionImp *const index) const {
    throw "ExpressionImpGround::select N/A";
  }

  virtual ExpressionImp *store(const ExpressionImp *const array,
                               const ExpressionImp *const index,
                               const ExpressionImp *const value) const {
    throw "ExpressionImpGround::store N/A";
  }

};

// ---------------------------------------------------------------------------
class ExpressionImpGroundBoolean : public ExpressionImpGround {
public:
  explicit ExpressionImpGroundBoolean(bool b)
      : ExpressionImpGround(EK_ExpressionImpGroundBoolean), _b(b) {}

  static bool classof(const ExpressionImp *S) {
    return S->getKind() == EK_ExpressionImpGroundBoolean;
  }

  virtual bool getBooleanValue() const { return _b; }

  virtual bool isGroundBoolean() const { return true; }

  virtual unsigned long getUnsignedValue() const {
    throw "ExpressionImpGroundBoolean::getUnsignedValue N/A";
  }

  virtual bool checkSat() { return _b; }

  virtual bool addConstraint() { return _b; }

  virtual bool addTempConstraint() { return _b; }

  virtual void reset() {}

  virtual std::string getAssignment() const {
    //      return "";
    return ((_b == true) ? "1" : "0");
  }

  virtual ExpressionImp *booleanGround(bool x) const {
    return new ExpressionImpGroundBoolean(x);
  }

  virtual ExpressionImp *logicalAnd(const ExpressionImp *const e) const {
    return e->logicalAndGround(this);
  }

  virtual ExpressionImp *
  logicalAndGround(const ExpressionImpGroundBoolean *const e) const {
    return new ExpressionImpGroundBoolean(this->_b && e->_b);
  }

  virtual ExpressionImp *
      logicalAndSymbolic(const ExpressionImpSymbolic *const e) const;

  virtual ExpressionImp *logicalOr(const ExpressionImp *const e) const {
    return e->logicalOrGround(this);
  }

  virtual ExpressionImp *
  logicalOrGround(const ExpressionImpGroundBoolean *const e) const {
    return new ExpressionImpGroundBoolean(this->_b || e->_b);
  }

  virtual ExpressionImp *
      logicalOrSymbolic(const ExpressionImpSymbolic *const e) const;

  virtual ExpressionImp *logicalNot() const {
    return new ExpressionImpGroundBoolean(!(this->_b));
  }

  virtual ExpressionImp *eq(const ExpressionImp *const e) const {
    return e->eqGround(this);
  }

  using ExpressionImpGround::eqGround;

  virtual ExpressionImp *
  eqGround(const ExpressionImpGroundBoolean *const e) const {
    return new ExpressionImpGroundBoolean(this->_b == e->_b);
  }

  virtual ExpressionImp *eqSymbolic(const ExpressionImpSymbolic *const e) const;

  using ExpressionImpGround::neqGround;

  virtual ExpressionImp *neq(const ExpressionImp *const e) const {
    return e->neqGround(this);
  }

  virtual ExpressionImp *
  neqGround(const ExpressionImpGroundBoolean *const e) const {
    return new ExpressionImpGroundBoolean(this->_b != e->_b);
  }

  virtual ExpressionImp *
      neqSymbolic(const ExpressionImpSymbolic *const e) const;

  virtual ExpressionImp *ite(ExpressionImp *thenExpr, ExpressionImp *elseExpr,
                             size_t width) const {
    return this->_b ? thenExpr : elseExpr;
  }

  virtual ExpressionImp *zextimp(size_t width) const;

  virtual void print(std::ostream &ostr) const { ostr << _b; }

private:
  bool _b;
};

// ---------------------------------------------------------------------------
class ExpressionImpGroundInteger : public ExpressionImpGround {
public:

  explicit ExpressionImpGroundInteger(unsigned n, size_t width)
      : ExpressionImpGround(EK_ExpressionImpGroundInteger),
        _n(BitVector::fromInteger(n, width)) {}

  explicit ExpressionImpGroundInteger(signed int n, size_t width)
      : ExpressionImpGround(EK_ExpressionImpGroundInteger),
        _n(BitVector::fromInteger(n, width)) {}

  explicit ExpressionImpGroundInteger(unsigned long n, size_t width)
      : ExpressionImpGround(EK_ExpressionImpGroundInteger),
        _n(BitVector::fromInteger(n, width)) {}

  explicit ExpressionImpGroundInteger(signed long n, size_t width)
      : ExpressionImpGround(EK_ExpressionImpGroundInteger),
        _n(BitVector::fromInteger(n, width)) {}

  explicit ExpressionImpGroundInteger(unsigned short n, size_t width)
      : ExpressionImpGround(EK_ExpressionImpGroundInteger),
        _n(BitVector::fromInteger(n, width)) {}

  explicit ExpressionImpGroundInteger(signed short n, size_t width)
      : ExpressionImpGround(EK_ExpressionImpGroundInteger),
        _n(BitVector::fromInteger(n, width)) {}

  explicit ExpressionImpGroundInteger(const BitVector &n)
      : ExpressionImpGround(EK_ExpressionImpGroundInteger), _n(n) {}

  static bool classof(const ExpressionImp *S) {
    return S->getKind() == EK_ExpressionImpGroundInteger;
  }

  virtual bool isGroundInteger() const { return true; }

  virtual std::string getAssignment() const { return _n.toString(); }

  virtual bool getBooleanValue() const { throw "getBooleanValue N/A"; }

  virtual unsigned long getUnsignedValue() const { return _n.toUnsigned(); }

  size_t GetWidth() const { return _n.getNBits(); }

  virtual ExpressionImp *integerGround(unsigned int n, size_t width) const {
    return new ExpressionImpGroundInteger(n, width);
  }

  virtual ExpressionImp *integerGround(signed int n, size_t width) const {
    return new ExpressionImpGroundInteger(n, width);
  }

  virtual ExpressionImp *integerGround(unsigned long n, size_t width) const {
    return new ExpressionImpGroundInteger(n, width);
  }

  virtual ExpressionImp *integerGround(signed long n, size_t width) const {
    return new ExpressionImpGroundInteger(n, width);
  }

  virtual ExpressionImp *integerGround(unsigned short n, size_t width) const {
    return new ExpressionImpGroundInteger(n, width);
  }

  virtual ExpressionImp *integerGround(signed short n, size_t width) const {
    return new ExpressionImpGroundInteger(n, width);
  }

  virtual ExpressionImp *add(const ExpressionImp *const e) const {
    return e->addGround(this);
  }

  virtual ExpressionImp *
  addGround(const ExpressionImpGroundInteger *const e) const {
    return new ExpressionImpGroundInteger(this->_n + e->_n);
  }

  virtual ExpressionImp *
      addSymbolic(const ExpressionImpSymbolic *const e) const;

  virtual ExpressionImp *subtract(const ExpressionImp *const e) const {
    return e->subtractFromGround(this);
  }

  virtual ExpressionImp *
  subtractGround(const ExpressionImpGroundInteger *const e) const {
    return new ExpressionImpGroundInteger(this->_n - e->_n);
  }

  virtual ExpressionImp *
  subtractFromGround(const ExpressionImpGroundInteger *const e) const {
    return new ExpressionImpGroundInteger(e->_n - this->_n);
  }

  virtual ExpressionImp *
      subtractSymbolic(const ExpressionImpSymbolic *const e) const;
  virtual ExpressionImp *
      subtractFromSymbolic(const ExpressionImpSymbolic *const e) const;

  virtual ExpressionImp *mult(const ExpressionImp *const e) const {
    return e->multGround(this);
  }

  virtual ExpressionImp *
  multGround(const ExpressionImpGroundInteger *const e) const {
    return new ExpressionImpGroundInteger(this->_n * e->_n);
  }

  virtual ExpressionImp *
      multSymbolic(const ExpressionImpSymbolic *const e) const;

  virtual ExpressionImp *udivimp(const ExpressionImp *const e) const {
    return e->udivFromGround(this);
  }

  virtual ExpressionImp *
  udivGround(const ExpressionImpGroundInteger *const e) const {
    //      return new ExpressionImpGroundInteger(this->_n / e->_n);
    return new ExpressionImpGroundInteger(this->_n.udiv(e->_n));
  }

  virtual ExpressionImp *
  udivFromGround(const ExpressionImpGroundInteger *const e) const {
    //      return new ExpressionImpGroundInteger(e->_n / this->_n);
    return new ExpressionImpGroundInteger(e->_n.udiv(this->_n));

  }

  virtual ExpressionImp *
      udivSymbolic(const ExpressionImpSymbolic *const e) const;
  virtual ExpressionImp *
      udivFromSymbolic(const ExpressionImpSymbolic *const e) const;

  virtual ExpressionImp *sdivimp(const ExpressionImp *const e) const {
    return e->sdivFromGround(this);
  }

  virtual ExpressionImp *
  sdivGround(const ExpressionImpGroundInteger *const e) const {
    //      return new ExpressionImpGroundInteger(this->_n / e->_n);
    return new ExpressionImpGroundInteger(this->_n.sdiv(e->_n));
  }

  virtual ExpressionImp *
  sdivFromGround(const ExpressionImpGroundInteger *const e) const {
    //      return new ExpressionImpGroundInteger( e->_n / this->_n);
    return new ExpressionImpGroundInteger(e->_n.sdiv(this->_n));
  }

  virtual ExpressionImp *
      sdivSymbolic(const ExpressionImpSymbolic *const e) const;
  virtual ExpressionImp *
      sdivFromSymbolic(const ExpressionImpSymbolic *const e) const;

  virtual ExpressionImp *uremimp(const ExpressionImp *const e) const {
    return e->uremFromGround(this);
  }

  virtual ExpressionImp *
  uremGround(const ExpressionImpGroundInteger *const e) const {
    return new ExpressionImpGroundInteger(this->_n.urem(e->_n));
    //      return new ExpressionImpGroundInteger(this->_n % e->_n);
  }

  virtual ExpressionImp *
  uremFromGround(const ExpressionImpGroundInteger *const e) const {
    return new ExpressionImpGroundInteger(e->_n.urem(this->_n));
    //      return new ExpressionImpGroundInteger(e->_n % this->_n );
  }

  virtual ExpressionImp *
      uremSymbolic(const ExpressionImpSymbolic *const e) const;
  virtual ExpressionImp *
      uremFromSymbolic(const ExpressionImpSymbolic *const e) const;

  virtual ExpressionImp *sremimp(const ExpressionImp *const e) const {
    return e->sremFromGround(this);
  }

  virtual ExpressionImp *
  sremGround(const ExpressionImpGroundInteger *const e) const {
    return new ExpressionImpGroundInteger(this->_n.srem(e->_n));
    //      return new ExpressionImpGroundInteger(this->_n % e->_n);
  }

  virtual ExpressionImp *
  sremFromGround(const ExpressionImpGroundInteger *const e) const {
    return new ExpressionImpGroundInteger(e->_n.srem(this->_n));
    //      return new ExpressionImpGroundInteger(this->_n % e->_n);
  }

  virtual ExpressionImp *
      sremSymbolic(const ExpressionImpSymbolic *const e) const;
  virtual ExpressionImp *
      sremFromSymbolic(const ExpressionImpSymbolic *const e) const;

  virtual ExpressionImp *bitXor(const ExpressionImp *const e) const {
    return e->bitXorGround(this);
  }

  virtual ExpressionImp *
  bitXorGround(const ExpressionImpGroundInteger *const e) const {
    return new ExpressionImpGroundInteger(this->_n & e->_n);
  }

  virtual ExpressionImp *
      bitXorSymbolic(const ExpressionImpSymbolic *const e) const;

  virtual ExpressionImp *bitAnd(const ExpressionImp *const e) const {
    return e->bitAndGround(this);
  }

  virtual ExpressionImp *
  bitAndGround(const ExpressionImpGroundInteger *const e) const {
    return new ExpressionImpGroundInteger(this->_n & e->_n);
  }

  virtual ExpressionImp *
      bitAndSymbolic(const ExpressionImpSymbolic *const e) const;

  virtual ExpressionImp *bitOr(const ExpressionImp *const e) const {
    return e->bitOrGround(this);
  }

  virtual ExpressionImp *
  bitOrGround(const ExpressionImpGroundInteger *const e) const {
    return new ExpressionImpGroundInteger(this->_n | e->_n);
  }

  virtual ExpressionImp *
      bitOrSymbolic(const ExpressionImpSymbolic *const e) const;

  virtual ExpressionImp *bitNot() const {
    return new ExpressionImpGroundInteger(~(this->_n));
  }

  virtual ExpressionImp *shiftL(const ExpressionImp *const e) const {
    return e->shiftLGround(this);
  }

  virtual ExpressionImp *
  shiftLGround(const ExpressionImpGroundInteger *const e) const {
    return new ExpressionImpGroundInteger(e->_n << this->_n.toUnsigned());
  }

  virtual ExpressionImp *
  shiftLForGround(const ExpressionImpGroundInteger *const e) const {
    return new ExpressionImpGroundInteger(this->_n << e->_n.toUnsigned());
  }

  virtual ExpressionImp *
      shiftLSymbolic(const ExpressionImpSymbolic *const e) const;

  virtual ExpressionImp *
      shiftLForSymbolic(const ExpressionImpSymbolic *const e) const;

  virtual ExpressionImp *aShiftRimp(const ExpressionImp *const e) const {
    return e->aShiftRGround(this);
  }

  virtual ExpressionImp *
  aShiftRGround(const ExpressionImpGroundInteger *const e) const {
    //      return new ExpressionImpGroundInteger(e->_n >>
    // this->_n.toUnsigned());
    return new ExpressionImpGroundInteger(
        (e->_n).aShiftRight(this->_n.toUnsigned()));
  }

  virtual ExpressionImp *
  aShiftRForGround(const ExpressionImpGroundInteger *const e) const {
    //      return new ExpressionImpGroundInteger(this->_n >>
    // e->_n.toUnsigned());
    return new ExpressionImpGroundInteger((this->_n)
                                              .aShiftRight(e->_n.toUnsigned()));
  }

  virtual ExpressionImp *
      aShiftRSymbolic(const ExpressionImpSymbolic *const e) const;

  virtual ExpressionImp *
      aShiftRForSymbolic(const ExpressionImpSymbolic *const e) const;

  virtual ExpressionImp *
  lShiftRGround(const ExpressionImpGroundInteger *const e) const {
    //      return new ExpressionImpGroundInteger(e->_n >>
    // this->_n.toUnsigned());
    return new ExpressionImpGroundInteger(
        (e->_n).lShiftRight(this->_n.toUnsigned()));
  }

  virtual ExpressionImp *
  lShiftRForGround(const ExpressionImpGroundInteger *const e) const {
    //      return new ExpressionImpGroundInteger(this->_n >>
    // e->_n.toUnsigned());
    return new ExpressionImpGroundInteger((this->_n)
                                              .lShiftRight(e->_n.toUnsigned()));
  }

  virtual ExpressionImp *
      lShiftRSymbolic(const ExpressionImpSymbolic *const e) const;

  virtual ExpressionImp *
      lShiftRForSymbolic(const ExpressionImpSymbolic *const e) const;

  virtual ExpressionImp *ultimp(const ExpressionImp *const e) const {
    return e->ugtGround(this);
  }

  virtual ExpressionImp *
  ultGround(const ExpressionImpGroundInteger *const e) const {
    return new ExpressionImpGroundBoolean(_n.ult(e->_n));
  }

  virtual ExpressionImp *
      ultSymbolic(const ExpressionImpSymbolic *const e) const;

  virtual ExpressionImp *ugtimp(const ExpressionImp *const e) const {
    return e->ultGround(this);
  }

  virtual ExpressionImp *
  ugtGround(const ExpressionImpGroundInteger *const e) const {
    return new ExpressionImpGroundBoolean(_n.ugt(e->_n));
  }

  virtual ExpressionImp *
      ugtSymbolic(const ExpressionImpSymbolic *const e) const;

  virtual ExpressionImp *uleimp(const ExpressionImp *const e) const {
    return e->ugeGround(this);
  }

  virtual ExpressionImp *
  uleGround(const ExpressionImpGroundInteger *const e) const {
    return new ExpressionImpGroundBoolean(_n.ule(e->_n));
  }

  virtual ExpressionImp *
      uleSymbolic(const ExpressionImpSymbolic *const e) const;

  virtual ExpressionImp *ugeimp(const ExpressionImp *const e) const {
    return e->uleGround(this);
  }

  virtual ExpressionImp *
  ugeGround(const ExpressionImpGroundInteger *const e) const {
    return new ExpressionImpGroundBoolean(_n.uge(e->_n));
  }

  virtual ExpressionImp *
      ugeSymbolic(const ExpressionImpSymbolic *const e) const;

  virtual ExpressionImp *sltimp(const ExpressionImp *const e) const {
    return e->sgtGround(this);
  }

  virtual ExpressionImp *
  sltGround(const ExpressionImpGroundInteger *const e) const {
    return new ExpressionImpGroundBoolean(_n.slt(e->_n));
  }

  virtual ExpressionImp *
      sltSymbolic(const ExpressionImpSymbolic *const e) const;

  virtual ExpressionImp *sgtimp(const ExpressionImp *const e) const {
    return e->sltGround(this);
  }

  virtual ExpressionImp *
  sgtGround(const ExpressionImpGroundInteger *const e) const {
    return new ExpressionImpGroundBoolean(_n.sgt(e->_n));
  }

  virtual ExpressionImp *
      sgtSymbolic(const ExpressionImpSymbolic *const e) const;

  virtual ExpressionImp *sleimp(const ExpressionImp *const e) const {
    return e->sgeGround(this);
  }

  virtual ExpressionImp *
  sleGround(const ExpressionImpGroundInteger *const e) const {
    return new ExpressionImpGroundBoolean(_n.sle(e->_n));
  }

  virtual ExpressionImp *
      sleSymbolic(const ExpressionImpSymbolic *const e) const;

  virtual ExpressionImp *sgeimp(const ExpressionImp *const e) const {
    return e->sleGround(this);
  }

  virtual ExpressionImp *
  sgeGround(const ExpressionImpGroundInteger *const e) const {
    return new ExpressionImpGroundBoolean(_n.sge(e->_n));
  }

  virtual ExpressionImp *
      sgeSymbolic(const ExpressionImpSymbolic *const e) const;

  virtual ExpressionImp *eq(const ExpressionImp *const e) const {
    return e->eqGround(this);
  }

  using ExpressionImpGround::eqGround;

  virtual ExpressionImp *
  eqGround(const ExpressionImpGroundInteger *const e) const {
    return new ExpressionImpGroundBoolean(this->_n == e->_n);
  }

  virtual ExpressionImp *eqSymbolic(const ExpressionImpSymbolic *const e) const;

  using ExpressionImpGround::neqGround;

  virtual ExpressionImp *neq(const ExpressionImp *const e) const {
    return e->neqGround(this);
  }

  virtual ExpressionImp *
  neqGround(const ExpressionImpGroundInteger *const e) const {
    return new ExpressionImpGroundBoolean(this->_n != e->_n);
  }

  virtual ExpressionImp *
      neqSymbolic(const ExpressionImpSymbolic *const e) const;

  virtual ExpressionImp *zextimp(size_t width) const {
    //      return this->zextGround(width);
    return new ExpressionImpGroundInteger((this->_n.zext(width)));
  }
  //    virtual ExpressionImp* zextGround(size_t width) const {
  //      return new ExpressionImpGroundInteger((this->_n.zext(width)));
  //    }

  virtual ExpressionImp *sextimp(size_t width) const {
    //      return this->sextGround(width);
    return new ExpressionImpGroundInteger((this->_n.sext(width)));

  }
  //    virtual ExpressionImp* sextGround(size_t width) const {
  //      return new ExpressionImpGroundInteger((this->_n.sext(width)));
  //    }

  virtual ExpressionImp *extractimp(size_t low, size_t high) const {
    //      return this->zextGround(width);
    return new ExpressionImpGroundInteger((this->_n.extract(low, high)));
  }

  virtual void print(std::ostream &ostr) const { ostr << _n.toString(); }

private:
  BitVector _n;
};

// ---------------------------------------------------------------------------
class ExpressionImpSymbolic : public ExpressionImp {
public:

  ExpressionImpSymbolic() : ExpressionImp(EK_ExpressionImpSymbolic) {}
  ExpressionImpSymbolic(ExpressionImpKind k) : ExpressionImp(k) {}

  static bool classof(const ExpressionImp *S) {
    return (S->getKind() == EK_ExpressionImpBoolector ||
            S->getKind() == EK_ExpressionImpSymbolic ||
            S->getKind() == EK_ExpressionImpYices ||
            S->getKind() == EK_ExpressionImpZ3 ||
            S->getKind() == EK_ExpressionImpMathSAT);
  }

  //FIXME - na drugo mesto prebaciti
  virtual ExpressionImp *
  uninterpretedFunction(const Function &fun,
                        const std::vector<const ExpressionImp *> &args) {
    std::cout << "ExpressionImpSymbolic --- sta cu ovde " << fun.getName()
              << std::endl;
    throw "ExpressionImpSymbolic::uninterpretedFunction N/A";
  }

  virtual ExpressionImp *array(const std::string &name, size_t domain,
                               size_t range) {
    throw "ExpressionImpSymbolic::array N/A";
  }

  //FIXME - na drugo mesto prebaciti
  virtual ExpressionImp *select(const ExpressionImp *const array,
                                const ExpressionImp *const index) const {
    throw "ExpressionImpSymbolic::select N/A";
  }

  //FIXME - na drugo mesto prebaciti
  virtual ExpressionImp *store(const ExpressionImp *const array,
                               const ExpressionImp *const index,
                               const ExpressionImp *const value) const {
    throw "ExpressionImpSymbolic::store N/A";
  }

private:

  virtual ExpressionImp *add(const ExpressionImp *const e) const {
    return e->addSymbolic(this);
  }

  virtual ExpressionImp *subtract(const ExpressionImp *const e) const {
    return e->subtractFromSymbolic(this);
  }

  virtual ExpressionImp *mult(const ExpressionImp *const e) const {
    return e->multSymbolic(this);
  }

  virtual ExpressionImp *udivimp(const ExpressionImp *const e) const {
    return e->udivFromSymbolic(this);
  }

  virtual ExpressionImp *sdivimp(const ExpressionImp *const e) const {
    return e->sdivFromSymbolic(this);
  }

  virtual ExpressionImp *uremimp(const ExpressionImp *const e) const {
    return e->uremFromSymbolic(this);
  }

  virtual ExpressionImp *sremimp(const ExpressionImp *const e) const {
    return e->sremFromSymbolic(this);
  }

  virtual ExpressionImp *bitXor(const ExpressionImp *const e) const {
    return e->bitXorSymbolic(this);
  }

  virtual ExpressionImp *bitAnd(const ExpressionImp *const e) const {
    return e->bitAndSymbolic(this);
  }

  virtual ExpressionImp *logicalAnd(const ExpressionImp *const e) const {
    return e->logicalAndSymbolic(this);
  }

  virtual ExpressionImp *bitOr(const ExpressionImp *const e) const {
    return e->bitOrSymbolic(this);
  }

  virtual ExpressionImp *logicalOr(const ExpressionImp *const e) const {
    return e->logicalOrSymbolic(this);
  }

  virtual ExpressionImp *shiftL(const ExpressionImp *const e) const {
    return e->shiftLSymbolic(this);
  }

  virtual ExpressionImp *aShiftRimp(const ExpressionImp *const e) const {
    return e->aShiftRSymbolic(this);
  }

  virtual ExpressionImp *lShiftRimp(const ExpressionImp *const e) const {
    return e->lShiftRSymbolic(this);
  }

  virtual ExpressionImp *ultimp(const ExpressionImp *const e) const {
    return e->ugtSymbolic(this);
  }

  virtual ExpressionImp *ugtimp(const ExpressionImp *const e) const {
    return e->ultSymbolic(this);
  }

  virtual ExpressionImp *uleimp(const ExpressionImp *const e) const {
    return e->ugeSymbolic(this);
  }

  virtual ExpressionImp *ugeimp(const ExpressionImp *const e) const {
    return e->uleSymbolic(this);
  }

  virtual ExpressionImp *sltimp(const ExpressionImp *const e) const {
    return e->sgtSymbolic(this);
  }

  virtual ExpressionImp *sgtimp(const ExpressionImp *const e) const {
    return e->sltSymbolic(this);
  }

  virtual ExpressionImp *sleimp(const ExpressionImp *const e) const {
    return e->sgeSymbolic(this);
  }

  virtual ExpressionImp *sgeimp(const ExpressionImp *const e) const {
    return e->sleSymbolic(this);
  }

  virtual ExpressionImp *eq(const ExpressionImp *const e) const {
    return e->eqSymbolic(this);
  }

  virtual ExpressionImp *neq(const ExpressionImp *const e) const {
    return e->neqSymbolic(this);
  }

  virtual ExpressionImp *ite(ExpressionImp *thenExpr, ExpressionImp *elseExpr,
                             size_t width) const {
    if (thenExpr == NULL || elseExpr == NULL) {
      std::cerr << "thenExpr==NULL || elseExpr==NULL" << std::endl;
      return NULL;
    }
    ExpressionImpGround *gthen = llvm::dyn_cast<ExpressionImpGround>(thenExpr);
    ExpressionImpGround *gelse = llvm::dyn_cast<ExpressionImpGround>(elseExpr);
    return ite_(
        gthen == 0 ? thenExpr : integerGround(gthen->getUnsignedValue(), width),
        gelse == 0 ? elseExpr
                   : integerGround(gelse->getUnsignedValue(), width));
  }

  virtual ExpressionImp *ite_(ExpressionImp *thenExpr,
                              ExpressionImp *elseExpr) const = 0;

  virtual bool isGround() const { return false; }

  virtual ExpressionImpGround *getGround() const { throw "getGround N/A"; }

  virtual bool getBooleanValue() const { throw "getBooleanValue N/A"; }

  virtual unsigned long getUnsignedValue() const {
    throw "ExpressionImpSymbolic::getUnsignedValue N/A";
  }

  //    virtual ExpressionImp* zextGround(size_t width) const {
  //      throw "ExpressionImpSymbolic::zextGround N/A";
  //    }

  //    virtual ExpressionImp* sextGround(size_t width) const {
  //      throw "ExpressionImpSymbolic::sextGround N/A";
  //    }

};

// ---------------------------------------------------------------------------
class ExpressionFactory {
private:
  ExpressionImp *_expr;

public:
  explicit ExpressionFactory(ExpressionImp *expr) : _expr(expr) {}

  virtual ~ExpressionFactory();

  virtual Expression booleanGround(bool x);
  virtual Expression integerGround(unsigned int x, size_t width);
  virtual Expression integerGround(signed int x, size_t width);
  virtual Expression integerGround(unsigned long x, size_t width);
  virtual Expression integerGround(signed long x, size_t width);
  virtual Expression integerGround(unsigned short x, size_t width);
  virtual Expression integerGround(signed short x, size_t width);
  virtual Expression booleanSymbolic(const std::string &name);
  virtual Expression unsignedSymbolic(const std::string &name, size_t width);
  virtual Expression signedSymbolic(const std::string &name, size_t width);

  virtual Expression uninterpretedFunction(const Function &f,
                                           const std::vector<Expression> &args);
  virtual Expression array(const std::string &name, size_t domain,
                           size_t range);

  virtual Expression select(const Expression &array,
                            const Expression &index) const;

  virtual Expression store(const Expression &array, const Expression &index,
                           const Expression &value) const;

};

} // namespace UrsaMajor

#endif
