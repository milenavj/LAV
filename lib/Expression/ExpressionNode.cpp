#include "expression/expressions/ExpressionNode.h"
#include "expression/output/OutputFormater.h"
#include "expression/output/Output.h"

#include <cassert>

namespace argo {

////////////////////////////////////////////////////////////////////////////////
////	Class ExpressionNode
////////////////////////////////////////////////////////////////////////////////

hash<const char*> ExpressionNode::pconstchar_hash_func;
hash<int>		   ExpressionNode::int_hash_func;
hash<long int>    ExpressionNode::long_int_hash_func;

#ifdef _DEBUG
std::vector<ExpressionNode*> ExpressionNode::_nodes;

void ExpressionNode::PrintAllNodes()
{
	cout<<"Size :: "<<ExpressionNode::_nodes.size()<<endl;

	std::vector<ExpressionNode*>::iterator i;
	for (i = _nodes.begin(); i != _nodes.end(); i++)
	{	printf("%p : ", (void*)*i);
		(*i)->Print(OutputFormater::GetInstance(SMT), std::cout);
		cout<<endl;
	}
}
#endif 

void ExpressionNode::Print(OutputFormater* formater, std::ostream& ostr) {
	  formater->PrintExpressionNode(this,ostr);	  
}


int ExpressionNode::compare(const ExpressionNode& e) const
{
        
//                coutput << "Comparing: " << *this << "  ::  "  << e << endl;
    if (this == &e) 
	return 0;

    if (GetType() < e.GetType())
	return -1;
    else if (GetType() > e.GetType())
	return 1;
                        
    return _name.compare(e._name);
}
}//end of namespace

#include "expression/expressions/NumeralNode.h"
namespace argo {

const RATIONAL& ExpressionNode::GetValueRational() const {
    throw "N/A";
}

const INT& ExpressionNode::GetValue() const {
    throw "N/A";
}
 
} //end of namespace
