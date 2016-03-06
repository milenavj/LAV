#ifndef __OUTPUT_FORMATER_H__
#define __OUTPUT_FORMATER_H__

#include <map>
#include <iostream>
namespace argo {
class ExpressionNode;
class OperatorNode;
}

#include "expression/output/OutputFormaterType.h"
namespace argo {
class OutputFormater
{
public:
     virtual void PrintExpressionNode(ExpressionNode* _node, std::ostream& ostr) = 0;
     virtual void PrintOperatorNode(OperatorNode* _node, std::ostream& ostr) = 0;

	 virtual ~OutputFormater()
     {}

     static OutputFormater* GetInstance(FORMATER type)
     {
	  return registry()[type];
     }
     
protected : 
     OutputFormater(FORMATER formater)
     {
	  registry()[formater] = this;
     }

    
     static std::map <int, OutputFormater*>& registry()
     {
	  static std::map <int, OutputFormater*> _registry;
	  return _registry;
     }
     
};

extern int printing_precedence[];

} //end of namespace

#endif
