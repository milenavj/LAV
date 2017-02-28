#ifndef __EXPRESSION_H
#define __EXPRESSION_H

namespace argo {
class SubstitutionArray;
class ExpressionSet;
class ExpressionNode;
}

#include "expression/expressions/ExpressionTypes.h"
#include "expression/expressions/ExpressionNode.h"
#include "expression/expressions/NumeralNode.h"
#include "expression/output/Output.h"

#include <iterator>
#include <vector>
#include <iostream>
#include <string>

namespace argo {

/**
* \brief Central class used for representation of First Order Logic Expressions
* (Terms and Formulae).
*
* Expression class implements the reference counted smart pointer functionality
* to ExpressionNodes.
* All common subexpressions of an expression are shared. All existing
* ExpressionNodes are registered
* in an ExpressionFactory. When a new Expression is created, ExpressionFactory
* is consulted. If a
* desired ExpressionNode already exists in the system, newly created Expression
* just points to it.
* \nosubgrouping
*/
class Expression {

  //FIXME
  friend class OperatorNode;

  /**
   * @name Construction, Destruction, Assignment
   */
  //@{
public:

  Expression() : _expression(0) {}

  ~Expression() { DetachExpressionNode(); }

  Expression(const Expression &e) : _expression(e._expression) {
    if (!e.IsNULL())
      _expression->IncreaseReferenceCount();
  }

  Expression &operator=(const Expression &e) {
    if (this != &e) {
      if (!e._expression) {
        _expression = 0;
        return *this;
      }
      e._expression->IncreaseReferenceCount();
      DetachExpressionNode();
      _expression = e._expression;
    }
    return *this;
  }

  Expression(ExpressionNode *e) { AttachExpressionNode(e); }

  ExpressionNode *_expression;

private:

  /**
   * Unique representative for the given ExpressionNode, given by the
   * ExpressionFactory, is attached to the Expression.
   */
  void AttachExpressionNode(ExpressionNode *e);

  /**
   *	Expression node is detached from the Expression. It is automatically
   * garbage-collected if
   *   its not needed anymore.
   */
  void DetachExpressionNode();

  //@}

  /** @name Expression Creation
   *  Static functions and operators used to create First Order Logic
   * Expressions (Terms and Formulae)
   */
  //@{
public:
  static Expression Variable(const std::string name, IntType t = noInt,
                             bool r = true);
  static Expression FormulaVariable(const std::string name);
  static Expression MetaVariable(const std::string name);
  static Expression Constant(const std::string name);
  static Expression Numeral(const INT &value, IntType intType = noInt);
  static Expression Numeral(const std::string name, IntType intType = noInt);
  static Expression RationalNumeral(const RATIONAL &value);
  static Expression RationalNumeral(const std::string name);
  static Expression IfThenElse(const Expression &formula,
                               const Expression &arg1, const Expression &arg2);
  static Expression Function(const std::string name,
                             const std::vector<Expression> &arguments,
                             IntType intType = noInt);
  static Expression Function(const std::string name, const Expression &arg1,
                             IntType intType = noInt);
  static Expression Function(const std::string name, const Expression &arg1,
                             const Expression &arg2, IntType intType = noInt);
  static Expression Function(const std::string name, const Expression &arg1,
                             const Expression &arg2, const Expression &arg3,
                             IntType intType = noInt);
  static Expression Function(const std::string name, const Expression &arg1,
                             const Expression &arg2, const Expression &arg3,
                             const Expression &arg4, IntType intType = noInt);
  static Expression Predicate(const std::string name,
                              const std::vector<Expression> &arguments);
  static Expression Predicate(const std::string name, const Expression &arg1);
  static Expression Predicate(const std::string name, const Expression &arg1,
                              const Expression &arg2);
  static Expression Predicate(const std::string name, const Expression &arg1,
                              const Expression &arg2, const Expression &arg3);
  static Expression Equality(const Expression &l, const Expression &r);
  static Expression Disequality(const Expression &l, const Expression &r);

  static Expression AND(const Expression &arg1, const Expression &arg2);
  static Expression AND(const Expression &arg1);
  static Expression AND(const std::vector<Expression> &arguments);

  static Expression OR(const Expression &arg1, const Expression &arg2);
  static Expression OR(const Expression &arg1);
  static Expression OR(const std::vector<Expression> &arguments);

  static Expression IMPL(const Expression &pre, const Expression &ant);
  static Expression IFF(const Expression &l, const Expression &r);
  static Expression NOT(const Expression &e);

  static Expression XOR(const Expression &arg1, const Expression &arg2);

  static Expression TOP();
  static Expression BOT();

  static Expression String(const std::string name);
  bool isString() const;

  static Expression gepi(const Expression &arg1, const Expression &arg2);
  static Expression add(const Expression &arg1, const Expression &arg2);
  static Expression sub(const Expression &arg1, const Expression &arg2);
  static Expression mul(const Expression &arg1, const Expression &arg2);
  static Expression udiv(const Expression &arg1, const Expression &arg2);
  static Expression sdiv(const Expression &arg1, const Expression &arg2);
  static Expression urem(const Expression &arg1, const Expression &arg2);
  static Expression srem(const Expression &arg1, const Expression &arg2);
  bool isGepi() const;
  bool isAdd() const;
  bool isSub() const;
  bool isMul() const;
  bool isUdiv() const;
  bool isSdiv() const;
  bool isUrem() const;
  bool isSrem() const;

  static Expression ult(const Expression &arg1, const Expression &arg2);
  static Expression ule(const Expression &arg1, const Expression &arg2);
  static Expression ugt(const Expression &arg1, const Expression &arg2);
  static Expression uge(const Expression &arg1, const Expression &arg2);
  static Expression slt(const Expression &arg1, const Expression &arg2);
  static Expression sle(const Expression &arg1, const Expression &arg2);
  static Expression sgt(const Expression &arg1, const Expression &arg2);
  static Expression sge(const Expression &arg1, const Expression &arg2);
  bool isUlt() const;
  bool isUle() const;
  bool isUgt() const;
  bool isUge() const;
  bool isSlt() const;
  bool isSle() const;
  bool isSgt() const;
  bool isSge() const;

  static Expression zext(const Expression &arg1, int width);
  static Expression sext(const Expression &arg1, int width);
  static Expression extract(const Expression &arg1, int low, int high);
  bool isZext() const;
  bool isSext() const;
  bool isExtract() const;

  static Expression bitNot(const Expression &arg1);
  static Expression bitAnd(const Expression &arg1, const Expression &arg2);
  static Expression bitOr(const Expression &arg1, const Expression &arg2);
  static Expression bitXor(const Expression &arg1, const Expression &arg2);
  static Expression ShiftL(const Expression &arg1, const Expression &arg2);
  static Expression lShiftR(const Expression &arg1, const Expression &arg2);
  static Expression aShiftR(const Expression &arg1, const Expression &arg2);
  bool isBitNot() const;
  bool isBitAnd() const;
  bool isBitOr() const;
  bool isBitXor() const;
  bool isShiftL() const;
  bool islShiftR() const;
  bool isaShiftR() const;

  Expression operator&&(const Expression &e) const;
  Expression operator||(const Expression &e) const;
  Expression operator!() const;

  static Expression select(const Expression &array, const Expression &index);
  bool isSelect() const;
  static Expression store(const Expression &array, const Expression &index,
                          const Expression &value);
  bool isStore() const;

  static Expression array(const Expression &name, const Expression &indexW,
                          const Expression &valueW);
  bool isArray() const;

  static Expression left(const Expression &e, IntType t);
  bool isLeft() const;

  static Expression right(const Expression &e, IntType t);
  bool isRight() const;

  static Expression tn(const Expression &e, const Expression &mem, IntType t);
  bool isTN() const;
  //@}

  /** @name Parsing and Printing
   */
  //@{
  /**
   *   Reads SMT-LIB syntax expression from an input stream.
   */
  static bool Read(std::istream &istr, Expression &expr);

  /**
   *   Reads SMT-LIB syntax expression from a string.
   */
  static bool Read(const std::string &str, Expression &expr);

  /**
   *   Prints the Expression to a given output stream using given
   * OutputFormater.
   */
  void Print(OutputFormater *formater, ostream &ostr) const {
    if (!IsNULL())
      _expression->Print(formater, ostr);
    else
      ostr << "(null)";
  }

  /**
   * Gives textual representation of the expression
   */
  std::string toString() const;

  //@}

  /** @name Type Checking
   *  Functions that are used to query the type of the expression
   */
  //@{

  /**
   *	Gets the expression's name i.e. its head. E.g. the function symbol, the
   * predicate symbol, connective symbol, etc.
   */
  const std::string &GetName() const {
    assert(!IsNULL());
    return _expression->GetName();
  }

  IntType getIntType() const {
    assert(!IsNULL());
    return _expression->getIntType();
  }

  bool isRelevant() const {
    assert(!IsNULL());
    return _expression->isRelevant();
  }
  unsigned short getIntWidth() const {
    assert(!IsNULL());
    return _expression->getIntWidth();
  }

  bool isSigned() const {
    assert(!IsNULL());
    return _expression->isSigned();
  }

  bool isUnsigned() const {
    assert(!IsNULL());
    return _expression->isSigned();
  }

  /**
   *	Queries the TYPE of the expression, given by EXPRESSION_TYPE enumeration.
   *  E.g. EXPR_AND, EXPR_VARIABLE, EXPR_EQUALITY, ...
   */
  EXPRESSION_TYPE GetType() const {
    assert(!IsNULL());
    return _expression->GetType();
  }

  /**
   *	Queries the sort of the expression. In a multisorted logic framework, each
   * Expression is given a unique sort.
   *  It is determined from the signature of each function and/or predicate
   * symbol and the sorts of its subexpressions.\
   *  E.g. If x, y : REAL and rel : REAL, REAL -> LOGICAL, than the sort of the
   * expression rel(x, y) is LOGICAL.
   */
  SORT GetSort() const {

    if (IsFormula())
      return LOGICAL_SORT;
    if (IsNumeral())
      return INT_SORT;

    return _expression->GetSort();
  }

  /**
   *	Checks if Expression is a variable
   */
  bool IsVariable() const { return GetType() == EXPR_VARIABLE; }

  /**
   *	Checks if Expression is a propositional variable
   */
  bool IsFormulaVariable() const { return GetType() == EXPR_FORMULA_VARIABLE; }

  /**
   *	Checks if Expression is a metavariable. Metavariables are used for
   * unification.
   */
  bool IsMetaVariable() const { return GetType() == EXPR_METAVARIABLE; }

  /**
   *	Checks if Expression is a constant.
   */
  bool IsConstant() const { return GetType() == EXPR_CONSTANT; }

  /**
   *	Checks if Expression is a function application.
   */
  bool IsFunction() const { return GetType() == EXPR_FUNCTION; }

  /**
   *	Checks if Expression is a numeral.
   */
  bool IsNumeral() const {
    return GetType() == EXPR_INT_NUMERAL || GetType() == EXPR_RATIONAL_NUMERAL;
  }

  /**
   *	Checks if Expression is a predicate.
   */
  bool IsPredicate() const { return GetType() == EXPR_PREDICATE; }

  /**
   *	Checks if Expression is an equality of terms.
   */
  bool IsEquality() const { return GetType() == EXPR_EQUALITY; }

  /**
   *	Checks if Expression is a disequality of terms.
   */
  bool IsDisequality() const { return GetType() == EXPR_DISEQUALITY; }

  /**
   *	Checks if Expression is a conjunction connective.
   */
  bool IsAND() const { return GetType() == EXPR_AND; }

  /**
   *	Checks if Expression is a disjunction connective.
   */
  bool IsOR() const { return GetType() == EXPR_OR; }

  bool IsXOR() const { return GetType() == EXPR_XOR; }

  /**
   *	Checks if Expression is an implication connective.
   */
  bool IsIMPL() const { return GetType() == EXPR_IMPL; }

  /**
   *	Checks if Expression is an equivalence connective.
   */
  bool IsIFF() const { return GetType() == EXPR_IFF; }

  /**
   *	Checks if Expression is a negation connective.
   */
  bool IsNOT() const { return GetType() == EXPR_NOT; }

  /**
   *	Checks if Expression is a TRUE logical constant.
   */
  bool IsTOP() const { return GetType() == EXPR_TOP; }

  /**
   *	Checks if Expression is a FALSE logical constant.
   */
  bool IsBOT() const { return GetType() == EXPR_BOT; }

  /**
   *      Checks if Expression is a term ITE
   */
  bool IsITE() const { return GetType() == EXPR_TERM_ITE; }

  /**
   *	Checks if Expression is a term (constant, variable, function).
   */
  bool IsTerm() const {
    assert(!IsNULL());
    return _expression->IsTerm();
  }

  /**
   *	Checks if Expression is atomic (predicate, logical constant)
   */
  bool IsAtom() const {
    assert(!IsNULL());
    return _expression->IsAtom();
  }

  /**
   *	Checks if Expression is a connective
   */
  bool IsConnective() const {
    assert(!IsNULL());
    return _expression->IsConnective();
  }

  /**
   *	Checks if Expression is a formula (atomic, connective)
   */
  bool IsFormula() const { return IsConnective() || IsAtom(); }

  /**
   *	Checks if Expression is a literal (atom or a negation of an atom)
   */
  bool IsLiteral() const {
    return IsAtom() || (IsNOT() && operator[](0).IsAtom());
  }

  /**
   *	Checks if Expression is a conjunction of literals
   */
  bool IsConjunctionOfLiterals() const {
    if (!IsAND())
      return false;
    Expression::operands_iterator i, iend = end();
    for (i = begin(); i != iend; i++)
      if (!i->IsLiteral())
        return false;
    return true;

  }

  /**
   *	Checks if Expression is a disjunction of literals
   */
  bool IsDisjunctionOfLiterals() const {
    if (!IsOR())
      return false;
    Expression::operands_iterator i, iend = end();
    for (i = begin(); i != iend; i++)
      if (!i->IsLiteral())
        return false;
    return true;

  }

  /**
   *	Checks if Expression is ground (contains no variables)
   */
  bool IsGround() const {
    assert(!IsNULL());
    return _expression->IsGround();
  }

  bool IsMetaGround() const {
    assert(!IsNULL());
    return _expression->IsMetaGround();
  }

  /**
   *	Checks if a node is attached to the expression
   */
  bool IsNULL() const { return _expression == 0; }

  //@}

public:
  /**
   * @name Operands manipulation
   */
  //@{
  typedef ExpressionNode::operands_iterator operands_iterator;

  /**
   *	Returns an operands_iterator that points to the first of the expression's
   * operands
   */
  operands_iterator begin() const { return _expression->begin(); }

  /**
   *	Returns an operands_iterator that points to the first positon pass the last
   * of the expression's operands
   */
  operands_iterator end() const { return _expression->end(); }

  /**
   *	Returns the number of expression's operands
   */
  size_t GetArity() const { return _expression->GetArity(); }

  /**
   *	Checks if expression has operands (is predicate, function or connective)
   */
  bool hasOperands() const { return _expression->hasOperands(); }

  /**
   *	Gets a const vector that contains expression's operands. If there are no
   * operands, empty vector is returned.
   */
  const std::vector<Expression> &GetOperands() const {
    assert(hasOperands());
    return _expression->GetOperands();
  }

  /**
   *	Access to num-th operand. Operand can't be changed.
   */
  const Expression &operator[](unsigned num) const {
    assert(hasOperands());
    assert(num < GetArity());

    return GetOperands()[num];
  }

  /**
   *	Changes the value of num-th expression's operand
   */
  void SetOperand(unsigned num, const Expression &e) {
    //	assert(hasOperands() && 0<=num && num<_expression->GetArity());
    assert(hasOperands() && num < _expression->GetArity());
    ExpressionNode *result = _expression->SetOperand(num, e);
    if (result != _expression) {
      DetachExpressionNode();
      AttachExpressionNode(result);
    }
  }

  /**
   *	Removes the num-th expression's operand (usefull for connectives)
   */
  void RemoveOperand(unsigned num) {
    //	assert(hasOperands() && 0<=num && num<_expression->GetArity());
    assert(hasOperands() && num < _expression->GetArity());
    ExpressionNode *result = _expression->RemoveOperand(num);
    if (result != _expression) {
      DetachExpressionNode();
      AttachExpressionNode(result);
    }
  }

  /**
   *	Sorts the operands of the expression in a "cannonical order".
   *  Used for "modulo commutativity" comparison.
   */
  void CannonizeOperandsOrder() {
    ExpressionNode *result = _expression->CannonizeOperandsOrder();
    if (result != _expression) {
      DetachExpressionNode();
      AttachExpressionNode(result);
    }
  }

  /**
   *	Creates the same expression with just some of its operands selected
   */
  Expression SelectOperands(const std::vector<unsigned> &indices) const {
    assert(hasOperands());
    return Expression(_expression->SelectOperands(indices));
  }

  //@}

  /**
   * @name Subexpressions manipulation
   */
  //@{

  /**
   *	Class used for iteration thrue all subexpressions of a given expression.
   */
  class subexpressions_iterator {
    friend class Expression;

  protected:
    subexpressions_iterator(const Expression &e) {
      _done = false;
      _stack.push_back(e);
    }

  public:

    subexpressions_iterator() { _done = true; }

    bool operator==(const subexpressions_iterator &pi) {
      return _done == pi._done;
    }

    bool operator!=(const subexpressions_iterator &pi) {
      return !(*this == pi);
    }

    bool operator++() {
      if (_stack.back().hasOperands()) {
        _stack.push_back(_stack.back()[0]);
        _current_position.push_back(0);
      } else {
        _stack.pop_back();
        if (_stack.empty()) {
          _done = true;
          return false;
        }

        _current_position.back()++;
        while (_current_position.back() >= _stack.back().GetArity()) {
          _stack.pop_back();
          if (_stack.empty())
            break;
          _current_position.pop_back();
          _current_position.back()++;
        }

        if (_stack.empty()) {
          _done = true;
          return false;
        }

        _stack.push_back(_stack.back()[_current_position.back()]);
      }
      return true;
    }

    const Expression &operator*() const { return _stack.back(); }

    Position GetPosition() const { return _current_position; }

    const Expression *operator->() const { return &operator*(); }

  protected:
    bool _done;
    std::vector<Expression> _stack;
    Position _current_position;

  };

  /**
   *	Gives the subexpressions_iterator that points to the first subexpression of
   * the Expression
   */
  subexpressions_iterator subexpressions_begin() const {
    return subexpressions_iterator(*this);
  }

  /**
   *	Gives the subexpressions_iterator that points to the first position after
   * the last subexpression of the Expression
   */
  subexpressions_iterator subexpressions_end() const {
    return subexpressions_iterator();
  }

  /**
   *  Extracts the subexpression at a given position.
   *  E.g. For expression  f(x, g(x, h(x, y), z(a)))  the subexpression
   *  at position (1, 1, 0) is x, and at position (1, 2) is z(a)
   */
  Expression operator[](Position position) const {
    if (position.empty())
      return *this;
    int i = position.front();
    position.pop_front();
    return (*this)[i][position];

  }

  /**
   *	Checks if a given expression occurs as a subexpression of a given
   * expression
   */
  bool ContainsSubexpression(const Expression &subexpression) const;
  bool ContainsSubexpression(const Expression &subexpression,
                             Position &position) const;

  /**
   *	Performs the substitution of all ocurrences of a given expression (in the
   * expression)
   *  with a given replacement
   */
  Expression Substitute(const Expression &expression,
                        const Expression &replacement) const {
    if (*this == expression)
      return replacement;
    if (!ContainsSubexpression(expression))
      return *this;

    ExpressionNode *subst = _expression->Substitute(expression, replacement);
    return _expression == subst ? *this : Expression(subst);
  }

  /**
   *  Replaces the expression at a given position with a given replacement
   */
  Expression Substitute(Position position,
                        const Expression &replacement) const {
    if (position.empty())
      return replacement;
    else {
      ExpressionNode *subst = _expression->Substitute(position, replacement);
      return _expression == subst ? *this : Expression(subst);
    }
  }

  /**
   *	A complexity measure for the expression. 1 for atoms, and (sum of operand
   * sizes+1) for compound expressions
   */
  int GetSize() const { return _expression->GetSize(); }

  /**
   *	A height of the expression tree
   */
  int GetDepth() const { return _expression->GetDepth(); }
  //@}

  /**
   * @name Comparison
   */
  //@{
  bool operator==(const Expression &e) const {
    if (IsNULL())
      return e.IsNULL();
    if (e.IsNULL())
      return IsNULL();

    return _expression == e._expression;
  }

  bool operator<(const Expression &e) const {
    assert(!IsNULL() && !e.IsNULL());
    return (*_expression) < (*e._expression);
  }

  bool operator!=(const Expression &e) const {
    return _expression != e._expression;
  }

  /**
   *	A simple, pointer based comparison
   */
  class _PTR_LT_class {
  public:
    bool operator()(const Expression &e1, const Expression &e2) const {
      return compare_ptr(e1, e2) < 0;
    }
  };

  static bool _PTR_LT(const Expression &e1, const Expression &e2) {
    return compare_ptr(e1, e2) < 0;
  }

  //@}

  /**
   * @name Union-Find structure
   */
  //@{
  static void Union(Expression &e1, Expression &e2) {
    e1.Find()._expression->SetUnionFindParrent(e2.Find()._expression);
  }

  Expression Find() const {
    Expression find;
    find._expression = _expression;
    while (find._expression->GetUnionFindParrent())
      find._expression = find._expression->GetUnionFindParrent();
    find._expression->IncreaseReferenceCount();
    return find;
  }

  void ResetUnionFind() { _expression->ResetUnionFindParrent(); }

  //@}

  /**
   * @name Other
   */
  //@{

  size_t hashCode() const { return _expression->hashCode(); }

  /**
   *	A numeric value of numeric expressions
   */
  const INT &GetValue() const {
    assert(IsNumeral());
    return _expression->GetValue();
  }

  const RATIONAL &GetValueRational() const {
    assert(IsNumeral());
    return _expression->GetValueRational();
  }

  unsigned getIndex() const {
    return _expression ? _expression->getIndex() : (unsigned)(-1);
  }

  void setIndex(unsigned index) { _expression->setIndex(index); }

  bool assigned() const {
    assert(!IsNULL());
    return _expression->assigned();
  }

  void setAssigned(bool assigned) {
    if (_expression)
      _expression->setAssigned(assigned);
  }

  Literal getLiteral() const {
    return _expression ? _expression->getLiteral() : (Literal)(-1);
  }

  void setLiteral(Literal literal) {
    if (_expression)
      _expression->setLiteral(literal);
  }

private:
  static int compare_ptr(const Expression &e1, const Expression &e2) {
    return e1._expression - e2._expression;
  }

  int compare(const Expression &e) const {
    assert(!IsNULL() && !e.IsNULL());
    return _expression->compare(*e._expression);
  }
};
} //end of namespace

#ifndef __EXPRESSION_INLINES__
#define __EXPRESSION_INLINES__
#include "ExpressionFactory.h"

namespace argo {
inline void Expression::DetachExpressionNode() {
  if (_expression && _expression->DecreaseReferenceCount() == 0) {
    ExpressionFactory::Instance()->Remove(_expression);
    _expression = 0;
  }
}

inline void Expression::AttachExpressionNode(ExpressionNode *e) {
  assert(e != 0);
  _expression = ExpressionFactory::Instance()->Get(e);
  _expression->IncreaseReferenceCount();
}

#endif

} //end of namespace

#endif
