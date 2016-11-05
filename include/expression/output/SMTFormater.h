#ifndef __SMT_FORMATER_H__
#define __SMT_FORMATER_H__

#include "expression/output/OutputFormater.h"

namespace argo {

class SMTFormater : public OutputFormater {
public:
  SMTFormater() : OutputFormater(SMT) {}

  virtual void PrintExpressionNode(ExpressionNode *_node, std::ostream &ostr);
  virtual void PrintOperatorNode(OperatorNode *_node, std::ostream &ostr);

private:
  std::string GetSymbol(ExpressionNode *e) const;
  void PrintPrefix(std::ostream &ostr, OperatorNode *e);
};

extern thread_local SMTFormater _smt_formater;
} // end of namespace

#endif
