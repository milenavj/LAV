#ifndef __EXPRESSION_NODE_H_
#define __EXPRESSION_NODE_H_

#include <vector>
#include <deque>
#include <string>
#include <sstream>
#include <iostream>
#include <cassert>
#include "expression/auxiliary/hash_set.h"

namespace argo { class OutputFormater; }

#include "expression/expressions/ExpressionTypes.h"
#include "expression/expressions/Sorts.h"
#include "expression/expressions/NumeralTypes.h"

namespace argo {

class Expression;
class ExpressionSet;

typedef unsigned short Literal;
typedef std::deque<unsigned> Position;

extern std::vector<Expression> dummy_vector;

#define PRIME_HASH 157

////////////////////////////////////////////////////////////////////////////////
////	Class ExpressionNode
////////////////////////////////////////////////////////////////////////////////
/**
* \brief Helper class used for representation of First Order Logic Expressions
* (Terms and Formulae). This
* is base class of a hierarchy and is used to represents Expressions that don't
* have children (atoms)
*
* This class should be hidden from the library end-users, and Expressions should
* be accessed only thrue
* Expression class (smart pointers)
* \nosubgrouping
*/
class ExpressionNode {
  friend class Expression;
  friend class ExpressionFactory;

  /**
   * @name Construction, Destruction, Cloning
   */
  //@{
public:
#ifdef _DEBUG
  thread_local static std::vector<ExpressionNode *> _nodes;

  static void PrintAllNodes();
#endif

  virtual ~ExpressionNode() {

//#ifdef _DEBUG
    std::vector<ExpressionNode *>::iterator i;
    for (i = _nodes.begin(); i != _nodes.end(); i++)
      if (*i == this) {
        _nodes.erase(i);
        break;
      }
//#endif
  }

  ExpressionNode(const ExpressionNode &e)
      : _reference_count(e._reference_count),
        _union_find_parrent(e._union_find_parrent), _index(e._index),
        _assigned(e._assigned), _literal(e._literal), _name(e._name),
        _type(e._type), _intType(e._intType) {
//#ifdef _DEBUG
    _nodes.push_back(this);
//#endif
  }

  ExpressionNode(EXPRESSION_TYPE type, const std::string &name,
                 IntType intType = noInt, bool r = true)
      : _reference_count(0), _union_find_parrent(0), _index((unsigned)(-1)),
        _assigned(false), _literal((Literal)(-1)), _name(name), _type(type),
        _intType(intType), _relevant(r) {
//#ifdef _DEBUG
    _nodes.push_back(this);
//#endif
  }

  virtual ExpressionNode *Clone() const { return new ExpressionNode(*this); }
  //@}

private:
  /**
   * @name Reference-counting
   */
  //@{
  void IncreaseReferenceCount() { ++_reference_count; }

  int DecreaseReferenceCount() {
    assert(_reference_count > 0);
    return --_reference_count;
  }

  int _reference_count;
  //@}

  /** @name Type Checking
   *  Functions that are used to query the type of the expression
   */
  //@{
public:
  virtual const std::string &GetName() { return _name; }

  EXPRESSION_TYPE GetType() const { return _type; }

  SORT GetSort() const {
    assert(IsTerm());
    return SortRegistry::Instance()->GetSort(_name);
  }

  virtual bool IsAND() { return GetType() == EXPR_AND; }

  virtual bool IsOR() { return GetType() == EXPR_OR; }

  virtual bool IsAtom() const {
    int type = GetType();
    return type == EXPR_EQUALITY || type == EXPR_DISEQUALITY ||
           type == EXPR_PREDICATE || type == EXPR_FORMULA_VARIABLE ||
           type == EXPR_METAVARIABLE || type == EXPR_TOP || type == EXPR_BOT;
  }

  virtual bool IsTerm() const {
    int type = GetType();
    return type == EXPR_CONSTANT || type == EXPR_FUNCTION ||
           type == EXPR_INT_NUMERAL || type == EXPR_RATIONAL_NUMERAL ||
           type == EXPR_VARIABLE || type == EXPR_METAVARIABLE ||
           type == EXPR_TERM_ITE;
  }

  virtual bool IsConnective() const {
    int type = GetType();
    return type == EXPR_AND || type == EXPR_OR || type == EXPR_IMPL ||
           type == EXPR_IFF || type == EXPR_NOT;
  }

  virtual bool IsGround() const { return _type != EXPR_VARIABLE; }

  virtual bool IsMetaGround() const { return _type != EXPR_METAVARIABLE; }
  //@}

  /**
   * @name Operands manipulation
   */
  //@{
  typedef std::vector<Expression>::const_iterator operands_iterator;

  operands_iterator begin() const { return GetOperands().begin(); }

  operands_iterator end() const { return GetOperands().end(); }

  virtual bool hasOperands() const { return false; }

  virtual size_t GetArity() const { return 0; }

  virtual const std::vector<Expression> &GetOperands() const {
    return dummy_vector;
  }

  virtual ExpressionNode *RemoveOperand(unsigned num) const {
    assert(0);
    throw "Operand manipulation function called on base ExpressionNode class";
  }

  virtual ExpressionNode *SetOperand(unsigned num, const Expression &e) const {
    assert(0);
    throw "Operand manipulation function called on base ExpressionNode class";
  }

  ExpressionNode *SelectOperands(const std::vector<unsigned> &indices) const {
    assert(0);
    throw "Operand manipulation function called on base ExpressionNode class";
  }

  virtual ExpressionNode *CannonizeOperandsOrder() { return this; }
  //@}

  /**
   * @name Subexpressions manipulation
   */
  //@{
  virtual ExpressionNode *Substitute(const Expression &expression,
                                     const Expression &replacement) {
    return this;
  }

  virtual ExpressionNode *Substitute(Position &position,
                                     const Expression &replacement) {
    return this;
  }

  virtual unsigned GetSize() const { return 1; }

  virtual unsigned GetDepth() const { return 1; }
  //@}

  /**
   * @name Comparison
   */
  //@{
  virtual int compare(const ExpressionNode &e) const;

  bool operator==(const ExpressionNode &e) const { return compare(e) == 0; }

  bool operator<(const ExpressionNode &e) const { return compare(e) < 0; }

  bool operator>(const ExpressionNode &e) const { return compare(e) > 0; }

  bool operator!=(const ExpressionNode &e) const { return !(*this == e); }
  //*}

  /** @name Printing
   */
  //@{
  virtual void Print(OutputFormater *formater, std::ostream &ostr);
  //@}

  virtual const RATIONAL &GetValueRational() const;
  virtual const INT &GetValue() const;

  /**
   * @name Union-Find structure
   */
  //@{
  ExpressionNode *GetUnionFindParrent() const { return _union_find_parrent; }

  void SetUnionFindParrent(ExpressionNode *parrent) {
    _union_find_parrent = parrent;
  }

  void ResetUnionFindParrent() { _union_find_parrent = 0; }

private:
  ExpressionNode *_union_find_parrent;
  //@}

public:
  unsigned getIndex() const { return _index; }

  void setIndex(unsigned index) { _index = index; }

private:
  unsigned _index;

public:
  bool assigned() const { return _assigned; }

  void setAssigned(bool assigned) { _assigned = assigned; }

private:
  bool _assigned;

public:
  Literal getLiteral() const { return _literal; }

  void setLiteral(Literal literal) { _literal = literal; }

private:
  Literal _literal;

public:

  /**
   * @name Other
   */
  //@{
  virtual const ExpressionSet &GetIndices() const {
    assert(0);
    throw "Operand manipulation function called on base ExpressionNode class";
  }

  virtual size_t hashCode() const {
    return int_hash_func(_type) * PRIME_HASH +
           pconstchar_hash_func(_name.c_str());
  }

public:
  IntType getIntType() const { return _intType; }

  void setIntType(IntType type) { _intType = type; }

  //    unsigned getIntWidth() const
  //    {return ((_intType<3) ? _intType : (_intType+1)/8)*8;}

  unsigned getIntWidth() const {
    return ((_intType < 3) ? 1 : ((_intType + 1) / 8) * 8);
  }

  bool isSigned() const { return ((_intType % 2) == 0); }

  bool isRelevant() const { return _relevant; }

  bool isUnsigned() const { return ((_intType % 2) == 1); }

protected:

  thread_local static hash<const char *> pconstchar_hash_func;
  thread_local static hash<int> int_hash_func;
  thread_local static hash<long int> long_int_hash_func;
  //@}

  std::string _name;
  EXPRESSION_TYPE _type;
  IntType _intType;
  bool _relevant; //ovo je krpljevina i ovome ovde nije mesto

};

} //end of namespace

#endif
