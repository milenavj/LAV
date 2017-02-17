#ifndef __NUMERAL_NODE_H__
#define __NUMERAL_NODE_H__

#include "expression/expressions/ExpressionNode.h"
#include "expression/expressions/NumeralTypes.h"

namespace argo {

class IntNumeralNode : public ExpressionNode {
  friend class Expression;

public:
  const INT &GetValue() const { return _value; }

  const RATIONAL &GetValueRational() const { return _rvalue; }

  void SetValue(const INT &value) {
    _value = value;
    _rvalue = value;
  }

  //FIXME ne moze to ovako
  size_t hashCode() const {
    return int_hash_func(_type) * PRIME_HASH +
           int_hash_func(_intType * 1000000 + _value.get_si());
  }

  virtual ExpressionNode *Clone() const { return new IntNumeralNode(*this); }

  const std::string &GetName() {
    if (_name == "") {
      SetName(_value);
    }
    return _name;
  }

  virtual int compare(const ExpressionNode &e) const;

protected:
  IntNumeralNode(const std::string &name, IntType intType = noInt)
      : ExpressionNode(EXPR_INT_NUMERAL, "", intType) {
    _name = name;
    SetValue(INT(name.c_str()));
  }

  IntNumeralNode(const INT &value, IntType intType = noInt)
      : ExpressionNode(EXPR_INT_NUMERAL, "", intType) {
    SetValue(value);
  }

  IntNumeralNode() : ExpressionNode(EXPR_INT_NUMERAL, "") { SetValue(INT(0)); }

  void SetName(const INT &value) {
    std::string name;
    std::ostringstream name_stream(name);
    name_stream << value;
    _name = name_stream.str();
  }

  INT _value;
  RATIONAL _rvalue;
};

class RationalNumeralNode : public ExpressionNode {
  friend class Expression;

public:
  const RATIONAL &GetValueRational() const { return _value; }

  void SetValue(const RATIONAL &value) { _value = value; }

  size_t hashCode() const {
    return int_hash_func(_type) * PRIME_HASH +
           int_hash_func(_value.get_num().get_si()) +
           int_hash_func(_value.get_den().get_si());
  }

  virtual ExpressionNode *Clone() const {
    return new RationalNumeralNode(*this);
  }

  const std::string &GetName() {
    if (_name == "") {
      SetName(_value);
    }
    return _name;
  }

  virtual int compare(const ExpressionNode &e) const;

protected:
  RationalNumeralNode(const std::string &name)
      : ExpressionNode(EXPR_RATIONAL_NUMERAL, "") {
    _value = RATIONAL(name.c_str());
    _name = name;
  }

  RationalNumeralNode(const RATIONAL &value)
      : ExpressionNode(EXPR_RATIONAL_NUMERAL, "") {
    SetValue(value);
  }

  RationalNumeralNode() : ExpressionNode(EXPR_RATIONAL_NUMERAL, "") {
    SetValue(RATIONAL(0));
  }

  void SetName(const RATIONAL &value) {
    std::string name;
    std::ostringstream name_stream(name);
    name_stream << value;
    _name = name_stream.str();
  }
  RATIONAL _value;
};

} //end of namespace

#endif
