#include "expression/auxiliary/Names.h"
#include <string>
#include <sstream>
#include <cstdlib>

namespace argo {

Names::Names(const std::string &prefix) : _prefix(prefix) { _max_num = 1; }

void Names::AddName(const std::string &name) {
  int prefix_len = _prefix.size();

  if (name.substr(0, prefix_len) == _prefix) {
    unsigned long n = atol(name.substr(prefix_len).c_str());
    if (n >= _max_num)
      _max_num = n + 1;
  }
}

std::string Names::GetName() {
  std::ostringstream s_name;
  s_name << _prefix << _max_num++;

  return s_name.str();
}

std::string Names::itoa(int i) {
  std::ostringstream s_name;
  s_name << i;
  return s_name.str();
}

} //end of namespace