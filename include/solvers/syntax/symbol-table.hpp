#if !defined __SYMBOL_TABLE_HPP__
#define __SYMBOL_TABLE_HPP__

#include <map>

namespace UrsaMajor {

template <class T> class SymbolTable {
public:
  typedef std::map<std::string, T> map_type;
  typedef typename map_type::const_iterator const_iterator;
  typedef typename map_type::iterator iterator;

  const_iterator begin() const { return _map.begin(); }

  const_iterator end() const { return _map.end(); }

  iterator begin() { return _map.begin(); }

  iterator end() { return _map.end(); }

  void clear() { _map.erase(_map.begin(), _map.end()); }

  void set(const std::string &var, const T &val) { _map[var] = val; }

  T get(const std::string &var) const {
    typename map_type::const_iterator i = _map.find(var);
    return i != _map.end() ? i->second : T();
  }

  bool contains(const std::string &var) const {
    typename map_type::const_iterator i = _map.find(var);
    return i != _map.end();
  }

private:
  map_type _map;
};
} // namespace UrsaMajor
#endif
