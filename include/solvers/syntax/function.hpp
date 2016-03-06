#ifndef __FUNCTION_HPP__
#define __FUNCTION_HPP__

#include <string>
#include <vector>
#include <tr1/memory>

#include <string>
#include "syntax-tree.hpp"
#include "typed-identifier.hpp"

namespace UrsaMajor {
  class Function {
  public:
    Function(Type type, std::string name, const std::vector<TypedId>& args, Node* body) 
      : _type(type), _name(name), _args(args), _body(body) {
    }

    const std::string& getName() const {
      return _name;
    }

//    const size_t getArity() const {
    size_t getArity() const {
      return _args.size();
    }

    const std::string& getArgumentName(size_t i) const {
      return _args[i].second;
    }

    Type getArgumentType(size_t i) const {
      return _args[i].first;
    }

    Node* getBody() const {
      return _body.get();
    }

    void setBody(Node* body) {
      _body = std::tr1::shared_ptr<Node>(body);
    }

    bool isDeclaration() const {
      return _body == 0;
    }
    
    Type getType() const {
      return _type;
    }

  private:
    Type _type;
    std::string _name;
    std::vector<TypedId> _args;
    std::tr1::shared_ptr<Node> _body;
  };
} // namespace UrsaMajor

#endif
