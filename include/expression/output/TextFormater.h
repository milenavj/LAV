#ifndef __TEXT_FORMATER_H__
#define __TEXT_FORMATER_H__

#include "expression/output/OutputFormater.h"
namespace argo {

class ExpressionNode;
class OperatorNode;

class TextFormater : public OutputFormater {
public:
  TextFormater() : OutputFormater(TEXT) {}

  virtual void PrintExpressionNode(ExpressionNode *_node, std::ostream &ostr);
  virtual void PrintOperatorNode(OperatorNode *_node, std::ostream &ostr);

protected:
  TextFormater(FORMATER formater) : OutputFormater(formater) {}

  void PrintPrefix(std::ostream &ostr, OperatorNode *e);
  void PrintInfix(std::ostream &ostr, OperatorNode *e);

  virtual std::string GetSymbol(ExpressionNode *e) const;
};

extern thread_local TextFormater _text_formater;
} //end of namespace

#endif
