#ifndef __OUTPUT_H__
#define __OUTPUT_H__

#include <iostream>
#include <string>

#include "expression/output/OutputFormaterType.h"
namespace argo {

class Expression;
class ExpressionNode;

class Output {
public:
  Output(std::ostream &ostr, FORMATER formater = TEXT)
      : _ostr(ostr), _suspend(false) {
    SetFormater(formater);
  }

  virtual void SetFormater(FORMATER formater) { _formater = formater; }

  void Suspend() { _suspend = true; }

  void Enable() { _suspend = false; }

  Output &operator<<(const Expression &e);
  Output &operator<<(ExpressionNode &e);

  Output &operator<<(int i) {
    if (!_suspend)
      _ostr << i;
    return *this;
  }

  Output &operator<<(unsigned i) {
    if (!_suspend)
      _ostr << i;
    return *this;
  }

  Output &operator<<(double d) {
    if (!_suspend)
      _ostr << d;
    return *this;
  }

  Output &operator<<(const char *s) {
    if (!_suspend)
      _ostr << s;
    return *this;
  }

  Output &operator<<(const std::string &s) {
    if (!_suspend)
      _ostr << s;
    return *this;
  }

  Output &operator<<(std::ostream &(*pf)(std::ostream &)) {
    if (!_suspend)
      (*pf)(_ostr);
    return *this;
  }

  operator std::ostream &() { return _ostr; }

protected:
  std::ostream &_ostr;
  FORMATER _formater;

  bool _suspend;
};

extern thread_local Output coutput;
extern thread_local Output cerror;
} //end of namespace
#endif
