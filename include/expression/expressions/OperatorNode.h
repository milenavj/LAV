#ifndef __OPERATOR_NODE_H_
#define __OPERATOR_NODE_H_

#include "expression/expressions/ExpressionNode.h"

#include <algorithm>
namespace argo {
////////////////////////////////////////////////////////////////////////////////
////	Class OperatorNode
////////////////////////////////////////////////////////////////////////////////
/** 
* \brief Helper class used for representation of First Order Logic Expressions (Terms and Formulae) with
* children (i.e. Functions and Predicates). 
*
* This class should be hidden from the library end-users. 
* \nosubgrouping
*/
class OperatorNode : public ExpressionNode
{
public:
     friend class Expression;

     virtual bool IsGround() const;
     virtual bool IsMetaGround() const;


     virtual size_t GetArity() const;



     virtual bool hasOperands() const
     {	
	  return true;
     }
	
     virtual const std::vector<Expression>& GetOperands() const
     {	
	  return _operands;
     }
	
     virtual ExpressionNode* RemoveOperand(unsigned num) const;
     virtual ExpressionNode* SetOperand(unsigned num, const Expression& e) const;
     ExpressionNode* SelectOperands(const std::vector<unsigned>& indices) const;
     virtual ExpressionNode* CannonizeOperandsOrder();
     virtual ExpressionNode* Substitute(const Expression& expression, const Expression& replacement);
     virtual ExpressionNode* Substitute(Position& position, const Expression& replacement);
	 virtual ExpressionNode* SetOperands(const std::vector<Expression>& operands) const;
	 
	
	 virtual int compare(const ExpressionNode& e) const;

     virtual void Print(OutputFormater* formater, std::ostream& ostr);

     virtual unsigned GetSize() const;
	 virtual unsigned GetDepth() const;
     virtual size_t hashCode() const;

protected:
	
     OperatorNode(EXPRESSION_TYPE type, 
		  const std::string& name,
		  const std::vector<Expression>& operands,
                  IntType intType = noInt)
	  :	ExpressionNode(type,name,intType), _operands(operands)
     {}


     virtual OperatorNode* Clone() const
     {	
	  return new OperatorNode(*this); 
     }

     std::vector<Expression> _operands;
};

////////////////////////////////////////////////////////////////////////////////
////	Class ANDNode
////////////////////////////////////////////////////////////////////////////////
class ANDNode : public OperatorNode
{
	friend class Expression;

protected:
	ANDNode(const std::vector<Expression>& operands)
		: OperatorNode(EXPR_AND, "and", operands)
	{}

	static ExpressionNode* Simplify(const std::vector<Expression>& operands);

public:
	virtual ANDNode* Clone() const
	{	return new ANDNode(*this);	}

	virtual ExpressionNode* SetOperands(const std::vector<Expression>& operands) const
	{
		return ANDNode::Simplify(operands);
	}

};


////////////////////////////////////////////////////////////////////////////////
////	Class ORNode
////////////////////////////////////////////////////////////////////////////////
class ORNode : public OperatorNode
{
    friend class Expression;
 protected:
    ORNode(const std::vector<Expression>& operands)
	: OperatorNode(EXPR_OR, "or", operands)
	{}

    static ExpressionNode* Simplify(const std::vector<Expression>& operands);
 public:
    virtual ORNode* Clone() const {
	return new ORNode(*this);	
    }	

    virtual ExpressionNode* SetOperands(const std::vector<Expression>& operands) const	{
	return ORNode::Simplify(operands);
    }
};
}
#endif


#ifndef __OPERATOR_NODE_H_INLINES__
#define __OPERATOR_NODE_H_INLINES__

#include "Expression.h"
namespace argo {
inline ExpressionNode* OperatorNode::Substitute(const Expression& expression, const Expression& replacement) {	
    std::vector<Expression> new_operands = _operands;

    std::vector<Expression>::iterator i, iend=new_operands.end();
    for(i = new_operands.begin(); i!=iend; ++i) {
	*i = i->Substitute(expression,replacement);
    }

    return SetOperands(new_operands);
}

inline ExpressionNode* OperatorNode::Substitute(Position& position, const Expression& replacement)
{	
    std::vector<Expression> new_operands = _operands;

    int i = position.front();
    position.pop_front();

    new_operands[i] = new_operands[i].Substitute(position, replacement);
    return SetOperands(new_operands);
}

} //end of namespace

#endif
