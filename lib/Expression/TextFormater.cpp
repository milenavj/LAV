#include "expression/output/TextFormater.h"
#include "expression/expressions/Expression.h"
#include "expression/expressions/ExpressionNode.h"
#include "expression/expressions/OperatorNode.h"

namespace argo {

std::string TextFormater::GetSymbol(ExpressionNode *e) const {
  switch (e->GetType()) {
  case EXPR_TOP:
    return "True";
  case EXPR_BOT:
    return "False";
    break;
  case EXPR_METAVARIABLE:
    return std::string("!") + e->GetName();

  case EXPR_AND:
    return "/\\";
  case EXPR_OR:
    return "\\/";
  case EXPR_IMPL:
    return "==>";
  case EXPR_IFF:
    return "<==>";
  case EXPR_NOT:
    return "!";

  case EXPR_DISEQUALITY:
    return "!=";

  case EXPR_PREDICATE:
    if (e->GetName() == "<=")
      return "<=";
    if (e->GetName() == ">=")
      return ">=";
  default:
    return e->GetName();
  }
}

void TextFormater::PrintExpressionNode(ExpressionNode *node,
                                       std::ostream &ostr) {
  ostr << GetSymbol(node);
}

void TextFormater::PrintOperatorNode(OperatorNode *node, std::ostream &ostr) {
  switch (node->GetType()) {

  case EXPR_AND:
  case EXPR_OR:
  case EXPR_IMPL:
  case EXPR_IFF:
  case EXPR_EQUALITY:
  case EXPR_DISEQUALITY:
    PrintInfix(ostr, node);
    break;

  case EXPR_FUNCTION:
    if (node->GetName() == "+" || node->GetName() == "-" ||
        node->GetName() == "*" || node->GetName() == "/")
      PrintInfix(ostr, node);
    else
      PrintPrefix(ostr, node);
    break;

  case EXPR_PREDICATE:
    if (node->GetName() == "<" || node->GetName() == ">" ||
        node->GetName() == "<=" || node->GetName() == ">=")
      PrintInfix(ostr, node);
    else
      PrintPrefix(ostr, node);
    break;

  case EXPR_NOT:
    PrintPrefix(ostr, node);
    break;

  case EXPR_UNIVERSALQUANTIFIER:
  case EXPR_EXISTENTIALQUANTIFIER:
    break;

  case EXPR_TERM_ITE:
    PrintPrefix(ostr, node);
    break;

  default:
    break;

  }
}

void TextFormater::PrintInfix(std::ostream &ostr, OperatorNode *e) {
  std::vector<Expression>::const_iterator i, iend = e->GetOperands().end();
  int prec = printing_precedence[e->GetType()];
  for (i = e->GetOperands().begin(); i != iend; i++) {
    int prec_arg = printing_precedence[i->GetType()];

    if (i != e->GetOperands().begin())
      ostr << " " << GetSymbol((ExpressionNode *)e) << " ";

    bool parenthesize =
        prec_arg < prec ||
        (prec_arg == prec && (i->GetName() == "+" || i->GetName() == "-"));

    if (parenthesize)
      ostr << "(";
    i->Print(this, ostr);
    if (parenthesize)
      ostr << ")";
  }
}

void TextFormater::PrintPrefix(std::ostream &ostr, OperatorNode *e) {
  ostr << GetSymbol(e);
  ostr << "(";

  std::vector<Expression>::const_iterator i, iend = e->GetOperands().end();
  for (i = e->GetOperands().begin(); i != iend; i++) {
    if (i != e->GetOperands().begin())
      ostr << ", ";
    i->Print(this, ostr);
  }
  ostr << ")";
}

thread_local TextFormater _text_formater;

}
