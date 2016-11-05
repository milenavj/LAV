#ifndef __EXPRESSION_FACTORY_H
#define __EXPRESSION_FACTORY_H

namespace argo {
class Expression;
class ExpressionSet;
class ExpressionNode;
}

#include "expression/expressions/ExpressionTypes.h"

#include <string>
#include <vector>
#include <cassert>
#include <iostream>
#include "expression/auxiliary/hash_set.h"
namespace argo {

class ExpressionFactory {
public:
  static ExpressionFactory *Instance() {
    if (ExpressionFactory::_expression_factory == 0)
      ExpressionFactory::_expression_factory = new ExpressionFactory();

    return ExpressionFactory::_expression_factory;
  }

  ~ExpressionFactory() {}
  ExpressionNode *Get(ExpressionNode *expr_node);
  void Remove(ExpressionNode *expression);

#ifdef _DEBUG
  void dump();
#endif

  size_t GetNumberOfExistingExpressions() { return _existing_nodes.size(); }

private:
  ExpressionFactory() {}

#if defined(__GNUCPP__)
  class equal_to_ExpressionNode {
  public:
    bool operator()(const ExpressionNode *e1, const ExpressionNode *e2) const;
  };

  class hash_ExpressionNode {
  public:
    size_t operator()(const ExpressionNode *e) const;
  };

  typedef hash_set<ExpressionNode *, hash_ExpressionNode,
                   equal_to_ExpressionNode> ExpressionNodePointerSet;
#elif defined(__MSVC__)
  class hash_compare_ExpressionNode : public hash_compare<Expression *> {
  public:
    size_t operator()(const ExpressionNode *e1) const;
    bool operator()(const ExpressionNode *e1, const ExpressionNode *e2) const;
  };
  typedef hash_set<ExpressionNode *, hash_compare_ExpressionNode>
      ExpressionNodePointerSet;
#endif

  //Registry that stores existing nodes
  ExpressionNodePointerSet _existing_nodes;

  //Singleton instance
  thread_local static ExpressionFactory *_expression_factory;

};
} //end of namespace
#endif

#ifndef __EXPRESSION_FACTORY_H_INLINES_
#define __EXPRESSION_FACTORY_H_INLINES_

#include "expression/expressions/ExpressionNode.h"
#include "expression/output/Output.h"

namespace argo {
inline void ExpressionFactory::Remove(ExpressionNode *expression) {
if((expression != 0) && (_existing_nodes.find(expression) != _existing_nodes.end()))  {
//     coutput<<"Removing : "<<*expression<<endl;

  assert(expression != 0);
  assert(_existing_nodes.find(expression) != _existing_nodes.end());

  _existing_nodes.erase(expression);
  delete expression;
}
else return;

}

#if defined(__GNUCPP__)

inline size_t ExpressionFactory::hash_ExpressionNode::operator()(
    const ExpressionNode *e) const {
  return e->hashCode();
}

inline bool ExpressionFactory::equal_to_ExpressionNode::operator()(
    const ExpressionNode *e1, const ExpressionNode *e2) const {
  return (*e1) == (*e2);
}

#elif defined(__MSVC__)

inline size_t ExpressionFactory::hash_compare_ExpressionNode::operator()(
    const ExpressionNode *e) const {
  return e->hashCode();
}

inline bool ExpressionFactory::hash_compare_ExpressionNode::operator()(
    const ExpressionNode *e1, const ExpressionNode *e2) const {
  return (*e1) < (*e2);
}
#endif
} //end of namespace

#endif
