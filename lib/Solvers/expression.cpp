#include "solvers/solver-interfaces/expression.hpp"

namespace UrsaMajor {

Expression::Expression(ExpressionImp *imp) {
  if (imp == NULL)
    return;
  // FIXME what type, bitwidth???
  if (llvm::dyn_cast<ExpressionImpSymbolic>(imp) && imp->isGround()) {
    setImp(imp->getGround());
    delete imp;
  } else {
    setImp(imp);
  }
}

Expression::Expression(std::tr1::shared_ptr<ExpressionImp> imp) : _imp(imp) {}

void Expression::setFactory(ExpressionFactory *factory) {
  //   if (_factory)
  //     delete _factory;
  _factory = factory;
}

Expression Expression::integerGround(unsigned int x, size_t width) {
  return _factory->integerGround(x, width);
}

Expression Expression::integerGround(signed int x, size_t width) {
  return _factory->integerGround(x, width);
}

Expression Expression::integerGround(unsigned long x, size_t width) {
  return _factory->integerGround(x, width);
}

Expression Expression::integerGround(signed long x, size_t width) {
  return _factory->integerGround(x, width);
}

Expression Expression::integerGround(unsigned short x, size_t width) {
  return _factory->integerGround(x, width);
}

Expression Expression::integerGround(signed short x, size_t width) {
  return _factory->integerGround(x, width);
}

Expression Expression::booleanGround(bool x) {
  return _factory->booleanGround(x);
}

Expression Expression::signedSymbolic(const std::string &name, size_t width) {
  return _factory->signedSymbolic(name, width);
}

Expression Expression::unsignedSymbolic(const std::string &name, size_t width) {
  return _factory->unsignedSymbolic(name, width);
}

Expression Expression::booleanSymbolic(const std::string &name) {
  return _factory->booleanSymbolic(name);
}

Expression
Expression::uninterpretedFunction(const Function &f,
                                  const std::vector<Expression> &args) {
  return _factory->uninterpretedFunction(f, args);
}

Expression Expression::array(const std::string &name, size_t domain,
                             size_t range) {
  return _factory->array(name, domain, range);
}

Expression Expression::select(const Expression &array,
                              const Expression &index) {
  return _factory->select(array, index);
}

Expression Expression::store(const Expression &array, const Expression &index,
                             const Expression &value) {
  return _factory->store(array, index, value);
}

Expression Expression::operator+(const Expression &expr) const {
  return Expression(((*this)._imp)->add(expr._imp.get()));
}

Expression Expression::operator-(const Expression &expr) const {
  return Expression(((*this)._imp)->subtract(expr._imp.get()));
}

Expression Expression::operator*(const Expression &expr) const {
  return Expression(((*this)._imp)->mult(expr._imp.get()));
}

Expression Expression::operator^(const Expression &expr) const {
  return Expression(((*this)._imp)->bitXor(expr._imp.get()));
}

Expression Expression::operator&(const Expression &expr) const {
  return Expression(((*this)._imp)->bitAnd(expr._imp.get()));
}

Expression Expression::operator&&(const Expression &expr) const {
  return Expression(((*this)._imp)->logicalAnd(expr._imp.get()));
}

Expression Expression::operator<<(const Expression &expr) const {
  return Expression(((*this)._imp)->shiftL(expr._imp.get()));
}

//  Expression Expression::operator>>(const Expression& expr) const {
//    return Expression(((*this)._imp)->shiftR(expr._imp.get()));
//  }

Expression Expression::aShiftR(const Expression &expr) const {
  return Expression(((*this)._imp)->aShiftRimp(expr._imp.get()));
}

Expression Expression::lShiftR(const Expression &expr) const {
  return Expression(((*this)._imp)->lShiftRimp(expr._imp.get()));
}

Expression Expression::operator|(const Expression &expr) const {
  return Expression(((*this)._imp)->bitOr(expr._imp.get()));
}

Expression Expression::operator||(const Expression &expr) const {
  return Expression(((*this)._imp)->logicalOr(expr._imp.get()));
}

Expression Expression::operator~() const {
  return Expression(((*this)._imp)->bitNot());
}

Expression Expression::operator!() const {
  return Expression(((*this)._imp)->logicalNot());
}

//  Expression Expression::operator<(const Expression& expr) const {
//    return Expression((*this)._imp->lt(expr._imp.get()));
//  }

//  Expression Expression::operator>(const Expression& expr) const {
//    return Expression((*this)._imp->gt(expr._imp.get()));
//  }

Expression Expression::udiv(const Expression &expr) const {
  return Expression((*this)._imp->udivimp(expr._imp.get()));
}

Expression Expression::sdiv(const Expression &expr) const {
  return Expression((*this)._imp->sdivimp(expr._imp.get()));
}

Expression Expression::urem(const Expression &expr) const {
  return Expression((*this)._imp->uremimp(expr._imp.get()));
}

Expression Expression::srem(const Expression &expr) const {
  return Expression((*this)._imp->sremimp(expr._imp.get()));
}

Expression Expression::ult(const Expression &expr) const {
  return Expression((*this)._imp->ultimp(expr._imp.get()));
}

Expression Expression::slt(const Expression &expr) const {
  return Expression((*this)._imp->sltimp(expr._imp.get()));
}

Expression Expression::ugt(const Expression &expr) const {
  return Expression((*this)._imp->ugtimp(expr._imp.get()));
}
Expression Expression::sgt(const Expression &expr) const {
  return Expression((*this)._imp->sgtimp(expr._imp.get()));
}

Expression Expression::ule(const Expression &expr) const {
  return Expression((*this)._imp->uleimp(expr._imp.get()));
}

Expression Expression::sle(const Expression &expr) const {
  return Expression((*this)._imp->sleimp(expr._imp.get()));
}

Expression Expression::uge(const Expression &expr) const {
  return Expression((*this)._imp->ugeimp(expr._imp.get()));
}
Expression Expression::sge(const Expression &expr) const {
  return Expression((*this)._imp->sgeimp(expr._imp.get()));
}

Expression Expression::zext(size_t width) const {
  return Expression(((*this)._imp)->zextimp(width));
}

Expression Expression::sext(size_t width) const {
  return Expression(((*this)._imp)->sextimp(width));
}

Expression Expression::extract(size_t low, size_t high) const {
  return Expression(((*this)._imp)->extractimp(low, high));
}

Expression Expression::operator==(const Expression &expr) const {
  return Expression((*this)._imp->eq(expr._imp.get()));
}

Expression Expression::operator!=(const Expression &expr) const {
  return Expression((*this)._imp->neq(expr._imp.get()));
}

Expression Expression::ite(const Expression &thenExpr,
                           const Expression &elseExpr, size_t width) const {
  ExpressionImp *res =
      (*this)._imp->ite(thenExpr._imp.get(), elseExpr._imp.get(), width);
  if (res == thenExpr._imp.get())
    return thenExpr;
  else if (res == elseExpr._imp.get())
    return elseExpr;
  else
    return Expression(res);
}

void Expression::print(std::ostream &ostr) const { _imp->print(ostr); }

unsigned long Expression::getUnsignedValue() const {
  return _imp->getUnsignedValue();
}

bool Expression::getBooleanValue() const { return _imp->getBooleanValue(); }

bool Expression::checkSat() { return _imp->checkSat(); }

bool Expression::addConstraint() { return _imp->addConstraint(); }

bool Expression::addTempConstraint() { return _imp->addTempConstraint(); }

void Expression::reset() { _imp->reset(); }

std::string Expression::getAssignment() const { return _imp->getAssignment(); }

std::ostream &operator<<(std::ostream &ostr, const Expression &e) {
  e.print(ostr);
  return ostr;
}

thread_local ExpressionFactory *Expression::_factory = 0;

ExpressionFactory::~ExpressionFactory() { delete _expr; }

Expression ExpressionFactory::booleanGround(bool x) {
  return Expression(_expr->booleanGround(x));
}

Expression ExpressionFactory::integerGround(unsigned int x, size_t width) {
  return Expression(_expr->integerGround(x, width));
}

Expression ExpressionFactory::integerGround(signed int x, size_t width) {
  return Expression(_expr->integerGround(x, width));
}

Expression ExpressionFactory::integerGround(unsigned long x, size_t width) {
  return Expression(_expr->integerGround(x, width));
}

Expression ExpressionFactory::integerGround(signed long x, size_t width) {
  return Expression(_expr->integerGround(x, width));
}

Expression ExpressionFactory::integerGround(unsigned short x, size_t width) {
  return Expression(_expr->integerGround(x, width));
}

Expression ExpressionFactory::integerGround(signed short x, size_t width) {
  return Expression(_expr->integerGround(x, width));
}

Expression ExpressionFactory::booleanSymbolic(const std::string &name) {
  return Expression(_expr->booleanSymbolic(name));
}

Expression ExpressionFactory::unsignedSymbolic(const std::string &name,
                                               size_t width) {
  return Expression(_expr->unsignedSymbolic(name, width));
}

Expression ExpressionFactory::signedSymbolic(const std::string &name,
                                             size_t width) {
  return Expression(_expr->signedSymbolic(name, width));
}

Expression
ExpressionFactory::uninterpretedFunction(const Function &f,
                                         const std::vector<Expression> &args) {
  std::vector<const ExpressionImp *> exps;
  std::vector<Expression>::const_iterator i;
  for (i = args.begin(); i != args.end(); i++) {
    exps.push_back(i->_imp.get());
  }
  return Expression(_expr->uninterpretedFunction(f, exps));
}

Expression ExpressionFactory::array(const std::string &name, size_t domain,
                                    size_t range) {
  return Expression(_expr->array(name, domain, range));
}

Expression ExpressionFactory::select(const Expression &array,
                                     const Expression &index) const {
  return Expression(_expr->select(array._imp.get(), index._imp.get()));
}

Expression ExpressionFactory::store(const Expression &array,
                                    const Expression &index,
                                    const Expression &value) const {
  Expression e = Expression(
      _expr->store(array._imp.get(), index._imp.get(), value._imp.get()));
  return e;
}

} // namespace UrsaMajor
