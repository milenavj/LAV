#include "expression/expressions/Expression.h"
#include "expression/expressions/OperatorNode.h"
#include "expression/output/OutputFormater.h"

namespace argo {

////////////////////////////////////////////////////////////////////////////////
////	Class OperatorNode
////////////////////////////////////////////////////////////////////////////////
bool OperatorNode::IsGround() const {
    Expression::operands_iterator i, iend = _operands.end();
    for (i = _operands.begin(); i != iend; i++)
	if (!i->IsGround())
	    return false;
    return true;
}

bool OperatorNode::IsMetaGround() const {
    Expression::operands_iterator i, iend = _operands.end();
    for (i = _operands.begin(); i != iend; i++)
	if (!i->IsMetaGround())
	    return false;
    return true;
}

ExpressionNode* OperatorNode::RemoveOperand(unsigned num) const { 
    assert( (_type == EXPR_FUNCTION ||
	     _type == EXPR_PREDICATE || 
	     _type == EXPR_AND ||
	     _type == EXPR_OR) &&
	    0<=num && num<GetArity());

    std::vector<Expression> new_operands = _operands;
    new_operands.erase(new_operands.begin()+num);

    assert(_operands.size()-1==new_operands.size());

    return SetOperands(new_operands);
}

ExpressionNode* OperatorNode::SetOperand(unsigned num, const Expression& e) const {
    assert( (_type == EXPR_FUNCTION ||
	     _type == EXPR_PREDICATE || 
	     _type == EXPR_EQUALITY || 
	     _type == EXPR_DISEQUALITY || 
	     _type == EXPR_AND ||
	     _type == EXPR_OR) &&
	    0<=num && num<GetArity());

    std::vector<Expression> new_operands = _operands;
    new_operands[num]=e;
    return SetOperands(new_operands);
}



ExpressionNode* OperatorNode::SelectOperands(const std::vector<unsigned>& indices) const {
	 std::vector<Expression> new_operands;
     for (unsigned i = 0; i<GetArity(); i++)
     {
	  for (unsigned j = 0; j<indices.size(); j++)
	       if (j == i)
		    new_operands.push_back(_operands[i]);
     }
     
     return SetOperands(new_operands);
}

ExpressionNode* OperatorNode::CannonizeOperandsOrder() {
		if (IsAND() || 
			IsOR() || 
			GetType() == EXPR_EQUALITY || 
			GetType() == EXPR_DISEQUALITY ||
			GetType() == EXPR_ARRAY_PARTIAL_EQUALITY)
		{
			std::vector<Expression> new_operands = GetOperands();
			std::vector<Expression>::iterator i, iend = new_operands.end();
			for (i = new_operands.begin(); i != iend; i++)
				i->CannonizeOperandsOrder();

			if (!IsOR())
				sort(new_operands.begin(), new_operands.end(), Expression::_PTR_LT);

			return SetOperands(new_operands);
		}
		
		return Clone();
}

ExpressionNode* OperatorNode::SetOperands(const std::vector<Expression>& operands) const {
    return new OperatorNode(_type, _name, operands);
}



size_t OperatorNode::GetArity() const {	
    return _operands.size();	
}


unsigned OperatorNode::GetSize() const {   
    unsigned size=1;
    Expression::operands_iterator i,
	iend = _operands.end();

    for (i = _operands.begin();i != iend; i++)
	size+=i->GetSize();
    return size;
}

unsigned OperatorNode::GetDepth() const {   
    unsigned depth = 1;
    Expression::operands_iterator i,
	iend = _operands.end();

    for (i = _operands.begin();i != iend; i++) {
	unsigned op_depth = i->GetDepth();
	if (op_depth + 1>depth)
	    depth = op_depth + 1;
    }
    return depth;
}

size_t OperatorNode::hashCode() const {	
     size_t hc = ExpressionNode::hashCode();
	
     std::vector<Expression>::const_iterator i = _operands.begin(),
	  iend = _operands.end();
     for ( ; i<iend; i++)
	  hc = hc * PRIME_HASH + long_int_hash_func(reinterpret_cast<unsigned long>(i->_expression));
     return hc;
}

int OperatorNode::compare (const ExpressionNode& e) const 
{
//	coutput << "Comparing: " << *this << "   with   " << e << endl;
    if (this == &e)
	return 0;

    int cmp = ExpressionNode::compare(e);
    if (cmp < 0)
	return -1;
    if (cmp > 0)
	return 1;

    const std::vector<Expression>& operands_1 = this->GetOperands();
    const std::vector<Expression>& operands_2 = e.GetOperands();

    unsigned n1 = operands_1.size(),
	n2 = operands_2.size();

    if (n1 < n2)
	return -1;
    else if (n1 > n2)
	return 1;

    unsigned i;
    for(i = 0; i < n1; i++) {
	int cmp = (operands_1[i]).compare(operands_2[i]);
	if (cmp<0)
	    return -1;
	if (cmp>0)
	    return 1;
    }		

    return 0;
}

void OperatorNode::Print(OutputFormater* formater, std::ostream& ostr) {
    formater->PrintOperatorNode(this,ostr);
}


////////////////////////////////////////////////////////////////////////////////
////	Class ANDNode
////////////////////////////////////////////////////////////////////////////////
ExpressionNode* ANDNode::Simplify(const std::vector<Expression>& operands)
{
    if (operands.empty())
	return new ExpressionNode(EXPR_TOP, "top");

    std::vector<Expression> new_operands;
    std::vector<Expression>::const_iterator i, iend = operands.end();
    for (i = operands.begin(); i != iend; i++) {
	// x && BOT = BOT
	if (i->IsBOT())
	    return new ExpressionNode(EXPR_BOT, "bot");

	// x && TOP = x
	if (i->IsTOP())
	    continue;

	//Flattening of nary AND
	if (i->IsAND())
	{	  
	    std::vector<Expression>::const_iterator j, jend = i->end();
	    for (j = i->begin(); j!=jend; j++)
		//Add arguments that don't already exist
		if (find(new_operands.begin(), new_operands.end(), *j) == new_operands.end())
		    new_operands.push_back(*j);
	}
	else
	    //Add arguments that don't already exist
	    if (find(new_operands.begin(), new_operands.end(), *i) == new_operands.end())
		new_operands.push_back(*i);
    }
	  
    if (new_operands.size() == 0)
	return new ExpressionNode(EXPR_TOP, "top");

    return new ANDNode(new_operands);
}

////////////////////////////////////////////////////////////////////////////////
////	Class ORNode
////////////////////////////////////////////////////////////////////////////////
ExpressionNode* ORNode::Simplify(const std::vector<Expression>& operands)
{
    if (operands.empty())
	return new ExpressionNode(EXPR_BOT, "bot");

    std::vector<Expression> new_operands;
    std::vector<Expression>::const_iterator i, iend = operands.end();
    for (i = operands.begin(); i != iend; i++) {
	// x || TOP = TOP
	if (i->IsTOP())
	    return new ExpressionNode(EXPR_TOP, "top");

	// x || BOT = x
	if (i->IsBOT())
	    continue;

	//Flattening of nary OR
	if (i->IsOR())
	{	  
	    std::vector<Expression>::const_iterator j, jend = i->end();
	    for (j = i->begin(); j!=jend; j++)
		//Add arguments that don't already exist
		if (find(new_operands.begin(), new_operands.end(), *j) == new_operands.end())
		    new_operands.push_back(*j);
	}
	else
	    //Add arguments that don't already exist
	    if (find(new_operands.begin(), new_operands.end(), *i) == new_operands.end())
		new_operands.push_back(*i);
    }
	  
    if (new_operands.size() == 0)
	return new ExpressionNode(EXPR_BOT, "bot");

    return new ORNode(new_operands);
}

}//end of namespace
