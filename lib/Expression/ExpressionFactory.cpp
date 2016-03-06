#include "expression/expressions/ExpressionFactory.h"
#include "expression/expressions/Expression.h"
#include <cassert>

namespace argo {

ExpressionFactory* ExpressionFactory::_expression_factory = 0;
std::vector<Expression> dummy_vector;
}
#include "expression/output/Output.h"

namespace argo {
ExpressionNode* ExpressionFactory::Get(ExpressionNode* expr_node)
{
//	coutput << _existing_nodes.size() << "/" << _existing_nodes.bucket_count() << endl;
     ExpressionNodePointerSet::const_iterator i = _existing_nodes.find(expr_node);

     if ( i != _existing_nodes.end() )
	 {
		 if (*i != expr_node)
			 delete expr_node;
		 return *i;
	 }
     else
     {	
	  _existing_nodes.insert(expr_node);
	  return expr_node;
     }
}

#ifdef _DEBUG
void ExpressionFactory::dump()
{
     ExpressionNodePointerSet::const_iterator i, iend = _existing_nodes.end();
     for (i = _existing_nodes.begin(); i!=iend; i++)
	  coutput<<*(*i)<<endl;
}
#endif

} // end of namespace



