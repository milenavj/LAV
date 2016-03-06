#include "expression/output/Output.h"
#include "expression/output/OutputFormater.h"
#include "expression/expressions/Expression.h"
#include "expression/expressions/ExpressionNode.h"

namespace argo {

Output coutput(std::cout);
Output cerror (std::cerr);


Output& Output::operator<<(const Expression& e)
{
     if (!_suspend)
	  e.Print(OutputFormater::GetInstance(_formater), _ostr);
     return *this;
}

Output& Output::operator<<(ExpressionNode& e)
{
     if (!_suspend)
	  e.Print(OutputFormater::GetInstance(_formater), _ostr);
     return *this;
}

}//end of namespace