#include "expression/output/SMTFormater.h"
#include "expression/expressions/Expression.h"
#include "expression/expressions/ExpressionNode.h"
#include "expression/expressions/OperatorNode.h"

namespace argo {

std::string SMTFormater::GetSymbol(ExpressionNode *e) const {
  switch (e->GetType()) {
  case EXPR_VARIABLE:
    return std::string("?") + e->GetName();
  case EXPR_METAVARIABLE:
    return std::string("!") + e->GetName();
  default:
    return e->GetName();
  }
}

void SMTFormater::PrintExpressionNode(ExpressionNode *node,
                                      std::ostream &ostr) {
  if (node->GetType() == EXPR_INT_NUMERAL ||
      node->GetType() == EXPR_RATIONAL_NUMERAL)
    ostr << "" << GetSymbol(node) << " ";
  else
    ostr << GetSymbol(node) << " ";
}

void SMTFormater::PrintOperatorNode(OperatorNode *node, std::ostream &ostr) {

  switch (node->GetType()) {
  case EXPR_AND:
  case EXPR_OR:
  case EXPR_IMPL:
  case EXPR_IFF:
  case EXPR_EQUALITY:
  case EXPR_DISEQUALITY:
  case EXPR_FUNCTION:
  case EXPR_PREDICATE:
  case EXPR_NOT:
  case EXPR_TERM_ITE:
    PrintPrefix(ostr, node);
    break;
  case EXPR_UNIVERSALQUANTIFIER:
  case EXPR_EXISTENTIALQUANTIFIER:
    break;

  default:
    ostr << GetSymbol((ExpressionNode *)node);

  }
}

void SMTFormater::PrintPrefix(std::ostream &ostr, OperatorNode *e) {
  ostr << "(" << GetSymbol(e) << " ";

  Expression::operands_iterator i, iend = e->GetOperands().end();
  for (i = e->GetOperands().begin(); i != iend; i++) {
    if (i != e->GetOperands().begin())
      ostr << " ";
    i->Print(this, ostr);
  }
  ostr << ")";
}

thread_local SMTFormater _smt_formater;

} //end of namespace
