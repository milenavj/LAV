#include <cassert>
#include <vector>
#include <iostream>
#include <sstream>

#include "expression/expressions/Expression.h"
//#include "expression/parsing/ExpressionParser.h"
#include "expression/expressions/ExpressionFactory.h"

#include "expression/expressions/OperatorNode.h"

namespace argo {

//Reading from a file
/*bool Expression::Read(std::istream& istr, Expression& expr)
{
    std::vector<Expression> assumptions;
    ExpressionParser::yyin=&istr;
    bool eof = false;
    while(!eof) {
	int token = ExpressionParser::yylex();
	switch(token) {
	    case FORMULA: {
		if (ExpressionParser::yyparse()!=0) {
		    ExpressionParser::yyin=NULL;
		    return false;
		}
		expr = *ExpressionParser::p_parsed_expression;
		delete ExpressionParser::p_parsed_expression;
		break;
	    }
	    case ASSUMPTION: {
		if (ExpressionParser::yyparse()!=0) {
		    ExpressionParser::yyin=NULL;
		    return false;
		}
		Expression assumption = *ExpressionParser::p_parsed_expression;
		delete ExpressionParser::p_parsed_expression;
//		coutput << "Assumption: " << assumption << endl;
		assumptions.push_back(assumption);
		break;
	    }
	    case 0:
		eof = true;
	}
    }
    if (!assumptions.empty()) {
	expr = expr && Expression::AND(assumptions);
    }
    ExpressionParser::yyin=NULL;
    return !(expr.IsNULL());
}

//Reading from a string
bool Expression::Read(const std::string& str, Expression& expr)
{
     std::istringstream istr(str.c_str());
     return  Read(istr, expr);
}
*/
std::string Expression::toString() const {
  std::ostringstream str_stream;
  Output str_output(str_stream);
  str_output << *this;
  return str_stream.str();
}

//Expression construction functions
Expression Expression::Constant(const std::string name) {
  return Expression(new ExpressionNode(EXPR_CONSTANT, name));
}

Expression Expression::Numeral(const INT &value, IntType intType) {
  return Expression(new IntNumeralNode(value, intType));
}

Expression Expression::Numeral(const std::string name, IntType intType) {
  return Expression(new IntNumeralNode(name, intType));
}

Expression Expression::RationalNumeral(const RATIONAL &value) {
  return Expression(new RationalNumeralNode(value));
}

Expression Expression::RationalNumeral(const std::string name) {
  return Expression(new RationalNumeralNode(name));
}

Expression Expression::Variable(const std::string name, IntType t, bool r) {
  return Expression(new ExpressionNode(EXPR_VARIABLE, name, t, r));
}

Expression Expression::FormulaVariable(const std::string name) {
  return Expression(new ExpressionNode(EXPR_FORMULA_VARIABLE, name));
}

Expression Expression::MetaVariable(const std::string name) {
  return Expression(new ExpressionNode(EXPR_METAVARIABLE, name));
}

Expression Expression::Function(const std::string name, const Expression &arg1,
                                IntType intType) {
  std::vector<Expression> operands;
  operands.push_back(arg1);
  return Expression::Function(name, operands, intType);
}

Expression Expression::Function(const std::string name, const Expression &arg1,
                                const Expression &arg2, IntType intType) {
  std::vector<Expression> operands;
  operands.push_back(arg1);
  operands.push_back(arg2);
  return Expression::Function(name, operands, intType);
}

Expression Expression::Function(const std::string name, const Expression &arg1,
                                const Expression &arg2, const Expression &arg3,
                                IntType intType) {
  std::vector<Expression> operands;
  operands.push_back(arg1);
  operands.push_back(arg2);
  operands.push_back(arg3);
  return Expression::Function(name, operands, intType);
}

Expression Expression::Function(const std::string name, const Expression &arg1,
                                const Expression &arg2, const Expression &arg3,
                                const Expression &arg4, IntType intType) {
  std::vector<Expression> operands;
  operands.push_back(arg1);
  operands.push_back(arg2);
  operands.push_back(arg3);
  operands.push_back(arg4);
  return Expression::Function(name, operands, intType);
}

Expression Expression::Function(const std::string name,
                                const std::vector<Expression> &operands,
                                IntType intType) {

#ifndef NDEBUG
  unsigned int i;
  for (i = 0; i < operands.size(); i++)
    assert(operands[i].IsTerm());
#endif

  return Expression(new OperatorNode(EXPR_FUNCTION, name, operands, intType));
}

Expression Expression::IfThenElse(const Expression &formula,
                                  const Expression &arg1,
                                  const Expression &arg2) {
  assert(formula.IsFormula());
  assert(arg1.IsTerm());
  assert(arg2.IsTerm());

  std::vector<Expression> operands;
  operands.push_back(formula);
  operands.push_back(arg1);
  operands.push_back(arg2);

  return Expression(new OperatorNode(EXPR_TERM_ITE, "ITE", operands));
}

Expression Expression::Predicate(const std::string name,
                                 const Expression &arg1) {
  std::vector<Expression> operands;
  operands.push_back(arg1);
  return Expression::Predicate(name, operands);
}

Expression Expression::Predicate(const std::string name, const Expression &arg1,
                                 const Expression &arg2) {
  std::vector<Expression> operands;
  operands.push_back(arg1);
  operands.push_back(arg2);
  return Expression::Predicate(name, operands);
}

Expression Expression::Predicate(const std::string name, const Expression &arg1,
                                 const Expression &arg2,
                                 const Expression &arg3) {
  std::vector<Expression> operands;
  operands.push_back(arg1);
  operands.push_back(arg2);
  operands.push_back(arg3);
  return Expression::Predicate(name, operands);
}

Expression Expression::Predicate(const std::string name,
                                 const std::vector<Expression> &operands) {
#ifndef NDEBUG
  unsigned int i;
  for (i = 0; i < operands.size(); i++)
    assert(operands[i].IsTerm());
#endif

  return Expression(new OperatorNode(EXPR_PREDICATE, name, operands));
}

Expression Expression::Equality(const Expression &l, const Expression &r) {
  assert(l.IsTerm() && r.IsTerm());
  std::vector<Expression> operands;
  operands.push_back(l);
  operands.push_back(r);
  return Expression(new OperatorNode(EXPR_EQUALITY, "=", operands));
}

Expression Expression::Disequality(const Expression &l, const Expression &r) {
  assert(l.IsTerm() && r.IsTerm());
  std::vector<Expression> operands;
  operands.push_back(l);
  operands.push_back(r);
  return Expression(new OperatorNode(EXPR_DISEQUALITY, "!=", operands));
}

Expression Expression::AND(const Expression &arg1, const Expression &arg2) {
  std::vector<Expression> operands;
  operands.push_back(arg1);
  operands.push_back(arg2);
  return AND(operands);
}

Expression Expression::AND(const Expression &arg1) {
  std::vector<Expression> operands;
  operands.push_back(arg1);
  return AND(operands);
}

pthread_mutex_t varandsimp = PTHREAD_MUTEX_INITIALIZER;

Expression Expression::AND(const std::vector<Expression> &operands) {
#ifndef NDEBUG
  unsigned int iarg;
  for (iarg = 0; iarg < operands.size(); iarg++)
    assert(operands[iarg].IsFormula());
#endif
  pthread_mutex_lock(&varandsimp);
  Expression e(ANDNode::Simplify(operands));
  pthread_mutex_unlock(&varandsimp);
  return e;
//  return Expression(ANDNode::Simplify(operands));
}

Expression Expression::OR(const Expression &arg1, const Expression &arg2) {
  std::vector<Expression> operands;
  operands.push_back(arg1);
  operands.push_back(arg2);
  return OR(operands);
}

Expression Expression::OR(const Expression &arg1) {
  std::vector<Expression> operands;
  operands.push_back(arg1);
  return OR(operands);
}

Expression Expression::OR(const std::vector<Expression> &operands) {
#ifndef NDEBUG
  unsigned int iarg;
  for (iarg = 0; iarg < operands.size(); iarg++)
    assert(operands[iarg].IsFormula());
#endif
  pthread_mutex_lock(&varandsimp);
  Expression e(ORNode::Simplify(operands));
  pthread_mutex_unlock(&varandsimp);
  return e;
//  return Expression(ORNode::Simplify(operands));
}

Expression Expression::IMPL(const Expression &pre, const Expression &ant) {
  assert(pre.IsFormula() && ant.IsFormula());

  std::vector<Expression> operands;
  operands.push_back(pre);
  operands.push_back(ant);
  return Expression(new OperatorNode(EXPR_IMPL, "impl", operands));
}

Expression Expression::IFF(const Expression &e1, const Expression &e2) {
  assert(e1.IsFormula() && e2.IsFormula());

  std::vector<Expression> operands;
  operands.push_back(e1);
  operands.push_back(e2);
  return Expression(new OperatorNode(EXPR_IFF, "iff", operands));
}

Expression Expression::NOT(const Expression &e) {
  assert(e.IsFormula());

  if (e.IsNOT())
    return e[0];
  if (e.IsBOT())
    return Expression::TOP();
  if (e.IsTOP())
    return Expression::BOT();

  std::vector<Expression> operands;
  operands.push_back(e);
  return Expression(new OperatorNode(EXPR_NOT, "not", operands));
}

Expression Expression::XOR(const Expression &arg1, const Expression &arg2) {
  assert(arg1.IsFormula());
  assert(arg2.IsFormula());

  std::vector<Expression> operands;
  operands.push_back(arg1);
  operands.push_back(arg2);

  return Expression(new OperatorNode(EXPR_XOR, "xor", operands));
}

Expression Expression::TOP() {
  return Expression(new ExpressionNode(EXPR_TOP, "top"));
}

Expression Expression::BOT() {
  return Expression(new ExpressionNode(EXPR_BOT, "bot"));
}

// Expression building operators
Expression Expression::operator&&(const Expression &e) const {
  if (IsNULL())
    return e;

  if (e.IsNULL())
    return *this;

  return Expression::AND(*this, e);
}

Expression Expression::operator||(const Expression &e) const {
  if (IsNULL())
    return e;

  if (e.IsNULL())
    return *this;

  return Expression::OR(*this, e);
}

Expression Expression::operator!() const { return Expression::NOT(*this); }

//Subexrpression finding
bool Expression::ContainsSubexpression(const Expression &subexpression) const {
  if (*this == subexpression)
    return true;

  if (hasOperands()) {
    Expression::operands_iterator i, iend = end();
    for (i = begin(); i != iend; i++) {
      if (i->ContainsSubexpression(subexpression))
        return true;
    }
  }
  return false;
}

bool Expression::ContainsSubexpression(const Expression &subexpression,
                                       Position &position) const {
  if (*this == subexpression)
    return true;

  if (hasOperands()) {
    Expression::operands_iterator i, iend = end();
    for (i = begin(); i != iend; i++) {
      position.push_back(i - begin());
      if (i->ContainsSubexpression(subexpression, position))
        return true;
      position.pop_back();
    }
  }
  return false;
}

Expression Expression::gepi(const Expression &arg1, const Expression &arg2) {
  return Expression::Function("gepi", arg1, arg2);
}

bool Expression::isGepi() const {
  return (IsFunction() && GetName() == "gepi");
}

Expression Expression::add(const Expression &arg1, const Expression &arg2) {
  return Expression::Function("add", arg1, arg2);
}

bool Expression::isAdd() const { return (IsFunction() && GetName() == "add"); }

Expression Expression::sub(const Expression &arg1, const Expression &arg2) {
  //ako ne postoji nikakvo pojednostavljenje
  return Expression::Function("sub", arg1, arg2);
}

bool Expression::isSub() const { return (IsFunction() && GetName() == "sub"); }

Expression Expression::mul(const Expression &arg1, const Expression &arg2) {
  return Expression::Function("mul", arg1, arg2);
}

bool Expression::isMul() const { return (IsFunction() && GetName() == "mul"); }

Expression Expression::udiv(const Expression &arg1, const Expression &arg2) {
  return Expression::Function("udiv", arg1, arg2);
}

bool Expression::isUdiv() const {
  return (IsFunction() && GetName() == "udiv");
}

Expression Expression::sdiv(const Expression &arg1, const Expression &arg2) {
  return Expression::Function("sdiv", arg1, arg2);
}
bool Expression::isSdiv() const {
  return (IsFunction() && GetName() == "sdiv");
}

Expression Expression::urem(const Expression &arg1, const Expression &arg2) {
  return Expression::Function("urem", arg1, arg2);
}
bool Expression::isUrem() const {
  return (IsFunction() && GetName() == "urem");
}

Expression Expression::srem(const Expression &arg1, const Expression &arg2) {
  return Expression::Function("srem", arg1, arg2);
}
bool Expression::isSrem() const {
  return (IsFunction() && GetName() == "srem");
}

Expression Expression::ult(const Expression &arg1, const Expression &arg2) {
  return Expression::Function("ult", arg1, arg2);
}

bool Expression::isUlt() const { return (IsFunction() && GetName() == "ult"); }

Expression Expression::ule(const Expression &arg1, const Expression &arg2) {
  return Expression::Function("ule", arg1, arg2);
}
bool Expression::isUle() const { return (IsFunction() && GetName() == "ule"); }

Expression Expression::ugt(const Expression &arg1, const Expression &arg2) {
  return Expression::Function("ugt", arg1, arg2);
}
bool Expression::isUgt() const { return (IsFunction() && GetName() == "ugt"); }

Expression Expression::uge(const Expression &arg1, const Expression &arg2) {
  return Expression::Function("uge", arg1, arg2);
}
bool Expression::isUge() const { return (IsFunction() && GetName() == "uge"); }

Expression Expression::slt(const Expression &arg1, const Expression &arg2) {
  return Expression::Function("slt", arg1, arg2);
}
bool Expression::isSlt() const { return (IsFunction() && GetName() == "slt"); }

Expression Expression::sle(const Expression &arg1, const Expression &arg2) {
  return Expression::Function("sle", arg1, arg2);
}
bool Expression::isSle() const { return (IsFunction() && GetName() == "sle"); }

Expression Expression::sgt(const Expression &arg1, const Expression &arg2) {
  return Expression::Function("sgt", arg1, arg2);
}

bool Expression::isSgt() const { return (IsFunction() && GetName() == "sgt"); }

Expression Expression::sge(const Expression &arg1, const Expression &arg2) {
  return Expression::Function("sge", arg1, arg2);
}

bool Expression::isSge() const { return (IsFunction() && GetName() == "sge"); }

Expression Expression::select(const Expression &arg1, const Expression &arg2) {
  return Expression::Function("select", arg1, arg2);
}

bool Expression::isSelect() const {
  return (IsFunction() && GetName() == "select");
}

Expression Expression::store(const Expression &array, const Expression &index,
                             const Expression &value) {
  return Expression::Function("store", array, index, value);
}

bool Expression::isStore() const {
  return (IsFunction() && GetName() == "store");
}

Expression Expression::array(const Expression &name, const Expression &indexW,
                             const Expression &valueW) {
  return Expression::Function("array", name, indexW, valueW);
}

bool Expression::isArray() const {
  return (IsFunction() && GetName() == "array");
}

Expression Expression::left(const Expression &e, IntType t) {
  return Expression::Function("left", e, t);
}

bool Expression::isLeft() const {
  return (IsFunction() && GetName() == "left");
}

Expression Expression::tn(const Expression &e, const Expression &mem,
                          IntType t) {
  return Expression::Function("tn", e, mem, t);
}

bool Expression::isTN() const { return (IsFunction() && GetName() == "tn"); }

Expression Expression::right(const Expression &e, IntType t) {
  return Expression::Function("right", e, t);
}

bool Expression::isRight() const {
  return (IsFunction() && GetName() == "right");
}

Expression Expression::String(const std::string name) {
  return Expression::Function("string", Expression::Variable(name));
}

bool Expression::isString() const {
  return (IsFunction() && GetName() == "string");
}

Expression Expression::zext(const Expression &arg1, int width) {
  return Expression::Function("zext", arg1, Expression::Numeral(width));
}

bool Expression::isZext() const {
  return (IsFunction() && GetName() == "zext");
}

Expression Expression::sext(const Expression &arg1, int width) {
  return Expression::Function("sext", arg1, Expression::Numeral(width));
}

bool Expression::isSext() const {
  return (IsFunction() && GetName() == "sext");
}

Expression Expression::extract(const Expression &arg1, int low, int high) {
  return Expression::Function("extract", arg1, Expression::Numeral(low),
                              Expression::Numeral(high));
}

bool Expression::isExtract() const {
  return (IsFunction() && GetName() == "extract");
}

Expression Expression::bitNot(const Expression &arg1) {
  return Expression::Function("bitnot", arg1);
}

bool Expression::isBitNot() const {
  return (IsFunction() && GetName() == "bitnot");
}

Expression Expression::bitAnd(const Expression &arg1, const Expression &arg2) {
  return Expression::Function("bitand", arg1, arg2);
}

bool Expression::isBitAnd() const {
  return (IsFunction() && GetName() == "bitand");
}

Expression Expression::bitOr(const Expression &arg1, const Expression &arg2) {
  return Expression::Function("bitor", arg1, arg2);
}

bool Expression::isBitOr() const {
  return (IsFunction() && GetName() == "bitor");
}

Expression Expression::bitXor(const Expression &arg1, const Expression &arg2) {
  return Expression::Function("bitxor", arg1, arg2);
}

bool Expression::isBitXor() const {
  return (IsFunction() && GetName() == "bitxor");
}

Expression Expression::ShiftL(const Expression &arg1, const Expression &arg2) {
  return Expression::Function("shiftl", arg1, arg2);
}

bool Expression::isShiftL() const {
  return (IsFunction() && GetName() == "shiftl");
}

Expression Expression::lShiftR(const Expression &arg1, const Expression &arg2) {
  return Expression::Function("lshiftr", arg1, arg2);
}

bool Expression::islShiftR() const {
  return (IsFunction() && GetName() == "lshiftr");
}

Expression Expression::aShiftR(const Expression &arg1, const Expression &arg2) {
  return Expression::Function("ashiftr", arg1, arg2);
}

bool Expression::isaShiftR() const {
  return (IsFunction() && GetName() == "ashiftr");
}

}
