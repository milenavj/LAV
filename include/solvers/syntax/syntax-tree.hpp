#ifndef __SYNTAX_TREE_HPP__
#define __SYNTAX_TREE_HPP__

#include <iostream>
#include <vector>
#include <string>

#include "type.hpp"

namespace UrsaMajor {
  class SyntaxTreeVisitor;

  // ---------------------------------------------------------------------------
  class Node {
  public:
    virtual ~Node() {}
    virtual Node* copy() const = 0;
    virtual void acceptVisitor(SyntaxTreeVisitor& v) = 0;
  };


  // ---------------------------------------------------------------------------
  class NodeUnsigned : public Node {
  private:
    unsigned _n;
  public:
    NodeUnsigned(unsigned n) : _n(n) {}

    unsigned getValue() const {
      return _n;
    }

    virtual Node* copy() const {
      return new NodeUnsigned(*this);
    }

    virtual void acceptVisitor(SyntaxTreeVisitor& v);
  };

  // ---------------------------------------------------------------------------
  class NodeBoolean : public Node {
  private:
    bool _b;
  public:
    NodeBoolean(bool b) : _b(b) {}
    
    bool getValue() const {
      return _b;
    }

    virtual Node* copy() const {
      return new NodeBoolean(*this);
    }

    virtual void acceptVisitor(SyntaxTreeVisitor& v);
  };

  // ---------------------------------------------------------------------------
  class NodeVariable : public Node {
  public:

    NodeVariable(const std::string& name, Type type) 
      : _name(name), _type(type) {}
    
    const std::string& getName() const {
      return _name;
    }

    Type getType() const {
      return _type;
    }

    virtual Node* copy() const {
      return new NodeVariable(*this);
    }

    virtual void acceptVisitor(SyntaxTreeVisitor& v);

  private:
    std::string _name;
    Type _type;
  };

  // ---------------------------------------------------------------------------
  class NodeOperator : public Node {
  protected:
    std::vector<Node*> _ops;

  public:
    typedef std::vector<Node*>::const_iterator const_iterator;

    NodeOperator() {}

    NodeOperator(Node* n1) {
      _ops.push_back(n1);
    }

    NodeOperator(Node* n1, Node* n2) {
      _ops.push_back(n1);
      _ops.push_back(n2);
    }

    NodeOperator(Node* n1, Node* n2, Node* n3) {
      _ops.push_back(n1);
      _ops.push_back(n2);
      _ops.push_back(n3);
    }

    // shallow copy
    NodeOperator(const std::vector<Node*>& ops) 
      : _ops(ops) {
    }

    NodeOperator(const std::vector<Node*>& ops, Node* o) 
      : _ops(ops) {
      _ops.push_back(o);
    }

    ~NodeOperator() {
      for (std::vector<Node*>::iterator i = _ops.begin();
	   i != _ops.end();
	   i++)
	delete *i;
    }

    // TODO: assignment operator
    NodeOperator(const NodeOperator& n) {
      for (const_iterator i = n.begin(); i != n.end(); i++)
	_ops.push_back((*i)->copy());
    }

    const_iterator begin() const {
      return _ops.begin();
    }

    const_iterator end() const {
      return _ops.end();
    }

    Node* operator[] (size_t n) {
      return _ops[n];
    }

    size_t size() {
      return _ops.size();
    }
  };
  
  // ---------------------------------------------------------------------------
  class NodePlus : public NodeOperator {
  public:
    NodePlus(Node* n1, Node* n2) 
      : NodeOperator(n1, n2) {
    }

    NodePlus(const std::vector<Node*>& ops) 
      : NodeOperator(ops) {
    }

    virtual Node* copy() const {
      return new NodePlus(*this);
    }

    virtual void acceptVisitor(SyntaxTreeVisitor& v);
  };

  // ---------------------------------------------------------------------------
  class NodeMinus : public NodeOperator {
  public:
    NodeMinus(Node* n1, Node* n2) 
      : NodeOperator(n1, n2) {
    }

    NodeMinus(const std::vector<Node*>& ops) 
      : NodeOperator(ops) {
    }

    virtual Node* copy() const {
      return new NodeMinus(*this);
    }

    virtual void acceptVisitor(SyntaxTreeVisitor& v);
  };

  // ---------------------------------------------------------------------------
  class NodeAsterisk : public NodeOperator {
  public:
    NodeAsterisk(Node* n1, Node* n2) 
      : NodeOperator(n1, n2) {
    }

    NodeAsterisk(const std::vector<Node*>& ops) 
      : NodeOperator(ops) {
    }

    virtual Node* copy() const {
      return new NodeAsterisk(*this);
    }

    virtual void acceptVisitor(SyntaxTreeVisitor& v);
  };

  // ---------------------------------------------------------------------------
  class NodeLNot : public NodeOperator {
  public:
    NodeLNot(Node* n) 
      : NodeOperator(n) {
    }

    virtual Node* copy() const {
      return new NodeLNot(*this);
    }

    virtual void acceptVisitor(SyntaxTreeVisitor& v);
  };

  // ---------------------------------------------------------------------------
  class NodeBNot : public NodeOperator {
  public:
    NodeBNot(Node* n) 
      : NodeOperator(n) {
    }

    virtual Node* copy() const {
      return new NodeBNot(*this);
    }

    virtual void acceptVisitor(SyntaxTreeVisitor& v);
  };

  // ---------------------------------------------------------------------------
  class NodeBAnd : public NodeOperator {
  public:
    NodeBAnd(Node* n1, Node* n2) 
      : NodeOperator(n1, n2) {
    }

    NodeBAnd(const std::vector<Node*>& ops) 
      : NodeOperator(ops) {
    }

    virtual Node* copy() const {
      return new NodeBAnd(*this);
    }

    virtual void acceptVisitor(SyntaxTreeVisitor& v);
  };

  // ---------------------------------------------------------------------------
  class NodeBXor : public NodeOperator {
  public:
    NodeBXor(Node* n1, Node* n2) 
      : NodeOperator(n1, n2) {
    }

    NodeBXor(const std::vector<Node*>& ops) 
      : NodeOperator(ops) {
    }

    virtual Node* copy() const {
      return new NodeBXor(*this);
    }

    virtual void acceptVisitor(SyntaxTreeVisitor& v);
  };

  // ---------------------------------------------------------------------------
  class NodeLAnd : public NodeOperator {
  public:
    NodeLAnd(Node* n1, Node* n2) 
      : NodeOperator(n1, n2) {
    }

    NodeLAnd(const std::vector<Node*>& ops) 
      : NodeOperator(ops) {
    }

    virtual Node* copy() const {
      return new NodeLAnd(*this);
    }

    virtual void acceptVisitor(SyntaxTreeVisitor& v);
  };

  // ---------------------------------------------------------------------------
  class NodeBOr : public NodeOperator {
  public:
    NodeBOr(Node* n1, Node* n2) 
      : NodeOperator(n1, n2) {
    }

    NodeBOr(const std::vector<Node*>& ops) 
      : NodeOperator(ops) {
    }

    virtual Node* copy() const {
      return new NodeBOr(*this);
    }

    virtual void acceptVisitor(SyntaxTreeVisitor& v);
  };

  // ---------------------------------------------------------------------------
  class NodeLOr : public NodeOperator {
  public:
    NodeLOr(Node* n1, Node* n2) 
      : NodeOperator(n1, n2) {
    }

    NodeLOr(const std::vector<Node*>& ops) 
      : NodeOperator(ops) {
    }

    virtual Node* copy() const {
      return new NodeLOr(*this);
    }

    virtual void acceptVisitor(SyntaxTreeVisitor& v);
  };

  // ---------------------------------------------------------------------------
  class NodeShiftL : public NodeOperator {
  public:
    NodeShiftL(Node* n1, Node* n2) 
      : NodeOperator(n1, n2) {
    }

    virtual Node* copy() const {
      return new NodeShiftL(*this);
    }

    virtual void acceptVisitor(SyntaxTreeVisitor& v);
  };

  // ---------------------------------------------------------------------------
  class NodeShiftR : public NodeOperator {
  public:
    NodeShiftR(Node* n1, Node* n2) 
      : NodeOperator(n1, n2) {
    }

    virtual Node* copy() const {
      return new NodeShiftR(*this);
    }

    virtual void acceptVisitor(SyntaxTreeVisitor& v);
  };

  // ---------------------------------------------------------------------------
  class NodeGT : public NodeOperator {
  public:
    NodeGT(Node* n1, Node* n2)
      : NodeOperator(n1, n2) {
    }

    virtual Node* copy() const {
      return new NodeGT(*this);
    }

    virtual void acceptVisitor(SyntaxTreeVisitor& v);
  };

  // ---------------------------------------------------------------------------
  class NodeLT : public NodeOperator {
  public:
    NodeLT(Node* n1, Node* n2)
      : NodeOperator(n1, n2) {
    }

    virtual Node* copy() const {
      return new NodeLT(*this);
    }

    virtual void acceptVisitor(SyntaxTreeVisitor& v);
  };

  // ---------------------------------------------------------------------------
  class NodeEQ : public NodeOperator {
  public:
    NodeEQ(Node* n1, Node* n2)
      : NodeOperator(n1, n2) {
    }

    virtual Node* copy() const {
      return new NodeEQ(*this);
    }

    virtual void acceptVisitor(SyntaxTreeVisitor& v);
  };

  // ---------------------------------------------------------------------------
  class NodeNEQ : public NodeOperator {
  public:
    NodeNEQ(Node* n1, Node* n2)
      : NodeOperator(n1, n2) {
    }

    virtual Node* copy() const {
      return new NodeNEQ(*this);
    }

    virtual void acceptVisitor(SyntaxTreeVisitor& v);
  };

  // ---------------------------------------------------------------------------
  // Term ite operator
  class NodeITE : public NodeOperator {
  private:
    Type _type;

  public:
    NodeITE(Node* cond, Node* then_branch, Node* else_branch)
      : NodeOperator(cond, then_branch, else_branch), 
	_type(ERROR) {
    }

    NodeITE(Node* cond, Node* then_branch, Node* else_branch, Type type)
      : NodeOperator(cond, then_branch, else_branch), 
	_type(type) {
    }

    Type getType() const {
      return _type;
    }

    virtual Node* copy() const {
      return new NodeITE(*this);
    }

    virtual void acceptVisitor(SyntaxTreeVisitor& v);
  };


  // ---------------------------------------------------------------------------
  class NodeAssignment : public NodeOperator {
  private:
    Type _type;
    std::string _var;
  public: 
    NodeAssignment(const std::string& var, Type type, Node* expr) 
      : NodeOperator(expr), _type(type), _var(var) {
    }

    const std::string& getVar() const {
      return _var;
    }

//    const Type getType() const {
    Type getType() const {
      return _type;
    }

    virtual Node* copy() const {
      return new NodeAssignment(*this);
    }

    virtual void acceptVisitor(SyntaxTreeVisitor& v);
  };

  // ---------------------------------------------------------------------------
  class NodeArrayRead : public NodeOperator {
  private:
    std::string _var;
    Type _type;
  public: 
    NodeArrayRead(const std::string& var, const std::vector<Node*>& indices,
		  Type type) 
      : NodeOperator(indices), _var(var), _type(type) {
    }

    const std::string& getVar() const {
      return _var;
    }

    Type getType() const {
      return _type;
    }

    const_iterator beginIndices() const {
      return begin();
    }

    const_iterator endIndices() const {
      return end();
    }

    virtual Node* copy() const {
      return new NodeArrayRead(*this);
    }

    virtual void acceptVisitor(SyntaxTreeVisitor& v);
  };

  // ---------------------------------------------------------------------------
  class NodeArrayWrite : public NodeOperator {
  private:
    std::string _var;
    Type _type;

  public: 
    NodeArrayWrite(const std::string& var, 
		   const std::vector<Node*>& indices, Node* val,
		   Type type) 
      : NodeOperator(indices, val), _var(var), _type(type) {
    }

    const std::string& getVar() const {
      return _var;
    }

    Type getType() const {
      return _type;
    }

    const_iterator beginIndices() const {
      return begin();
    }

    const_iterator endIndices() const {
      return end() - 1;
    }

    Node* getValue() const {
      return *(end()-1);
    }

    virtual Node* copy() const {
      return new NodeArrayWrite(*this);
    }

    virtual void acceptVisitor(SyntaxTreeVisitor& v);
  };

  // ---------------------------------------------------------------------------
  class NodePrint : public NodeOperator {
  private:
    std::string _msg;

  public:
    NodePrint(const std::string& msg) 
      : NodeOperator(), _msg(msg) {
    }

    NodePrint(Node* expr) 
      : NodeOperator(expr) {
    }

    const std::string& getMessage() {
      return _msg;
    }

    virtual Node* copy() const {
      return new NodePrint(*this);
    }

    virtual void acceptVisitor(SyntaxTreeVisitor& v);
  };

  // ---------------------------------------------------------------------------
  class NodeAssert : public NodeOperator {
  private:
    bool _all_models;
  public:
    NodeAssert(Node* expr, bool all_models) 
      : NodeOperator(expr), _all_models(all_models) {
    }

    bool allModels() {
      return _all_models;
    }

    virtual Node* copy() const {
      return new NodeAssert(*this);
    }

    virtual void acceptVisitor(SyntaxTreeVisitor& v);
  };

  // ---------------------------------------------------------------------------
  class NodeWhile : public NodeOperator {
  public:
    NodeWhile(Node* cond, Node* body)
      : NodeOperator(cond, body) {
    }

    virtual Node* copy() const {
      return new NodeWhile(*this);
    }

    virtual void acceptVisitor(SyntaxTreeVisitor& v);
  };

  // ---------------------------------------------------------------------------
  class NodeExprStatement : public NodeOperator {
  public:
    NodeExprStatement(Node* expr) 
      : NodeOperator(expr) {
    }

    virtual Node* copy() const {
      return new NodeExprStatement(*this);
    }

    virtual void acceptVisitor(SyntaxTreeVisitor& v);
  };

  // ---------------------------------------------------------------------------
  class NodeIf : public NodeOperator {
  public:
    NodeIf(Node* cond, Node* body)
      : NodeOperator(cond, body) {
    }

    virtual Node* copy() const {
      return new NodeIf(*this);
    }

    virtual void acceptVisitor(SyntaxTreeVisitor& v);
  };

  // ---------------------------------------------------------------------------
  class NodeIfElse : public NodeOperator {
  public:
    NodeIfElse(Node* cond, Node* then_branch, Node* else_branch)
      : NodeOperator(cond, then_branch, else_branch) {
    }

    virtual Node* copy() const {
      return new NodeIfElse(*this);
    }

    virtual void acceptVisitor(SyntaxTreeVisitor& v);
  };

  // ---------------------------------------------------------------------------
  class NodeBlock : public NodeOperator {
  public:
    NodeBlock(const std::vector<Node*>& ops)
      : NodeOperator(ops) {
    }

    virtual Node* copy() const {
      return new NodeBlock(*this);
    }

    virtual void acceptVisitor(SyntaxTreeVisitor& v);
  };
  
  // ---------------------------------------------------------------------------
  class NodeFunctionCall : public NodeOperator {
  private:
    Type _type;
    std::string _name;
  public:
    NodeFunctionCall(const std::string& name, Type type, const std::vector<Node*>& arguments) 
      : NodeOperator(arguments), _type(type), _name(name) {
    }

    const std::string& getName() {
      return _name;
    }

    Type getType() const {
      return _type;
    }

    size_t getArity() const {
      return _ops.size();
    }
   
    virtual Node* copy() const {
      return new NodeFunctionCall(*this);
    }

    virtual void acceptVisitor(SyntaxTreeVisitor& v);
  };

} // namespace UrsaMajor

#endif
