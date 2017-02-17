#ifndef __SORT_REGISTRY_H__
#define __SORT_REGISTRY_H__

#include "expression/auxiliary/Names.h"

#include <cassert>
#include <map>
#include <string>
#include <vector>

namespace argo {
typedef enum {
  //Formula sort
  LOGICAL_SORT,

  //Term sorts
  REAL_SORT,
  INT_SORT,
  ARRAY_SORT
} SORT;

class SortRegistry {

public:

  std::string GetName(SORT sort) { return Instance()->names().GetName(); }

  static SortRegistry *Instance() {
    if (!_instance)
      _instance = new SortRegistry();
    return _instance;
  }

  void Register(const std::string &name, SORT sort_1) {
    std::vector<SORT> sorts;
    sorts.push_back(sort_1);
    Register(name, sorts);
  }

  void Register(const std::string &name, SORT sort_1, SORT sort_2) {
    std::vector<SORT> sorts;
    sorts.push_back(sort_1);
    sorts.push_back(sort_2);
    Register(name, sorts);
  }

  void Register(const std::string &name, SORT sort_1, SORT sort_2,
                SORT sort_3) {
    std::vector<SORT> sorts;
    sorts.push_back(sort_1);
    sorts.push_back(sort_2);
    sorts.push_back(sort_3);

    Register(name, sorts);
  }

  void Register(const std::string &name, SORT sort_1, SORT sort_2, SORT sort_3,
                SORT sort_4) {
    std::vector<SORT> sorts;
    sorts.push_back(sort_1);
    sorts.push_back(sort_2);
    sorts.push_back(sort_3);
    sorts.push_back(sort_4);

    Register(name, sorts);
  }

  void Register(const std::string &name, const std::vector<SORT> &sorts) {
    assert(registry().find(name) == registry().end());
    names().AddName(name);
    registry()[name] = sorts;
  }

  const std::vector<SORT> &GetSortVector(const std::string &name) const {
    assert(registry().find(name) != registry().end());
    return registry().find(name)->second;
  }

  SORT GetSort(const std::string &name) const {
    assert(registry().find(name) != registry().end());
    //#ifndef NDEBUG
    if (registry().find(name) == registry().end())
      std::cerr << "FATAL ERROR : " << name << " is not sorted" << endl;
    //#endif
    const std::vector<SORT> &sorts = registry().find(name)->second;
    return sorts[sorts.size() - 1];
  }

  static std::string GetFreshConstant(SORT s) {
    std::string name = names().GetName();
    return name;
  }

private:
  static std::map<std::string, std::vector<SORT> > &registry() {
    thread_local static std::map<std::string, std::vector<SORT> > _registry;
    return _registry;
  }

  static Names &names() {
    thread_local static Names _names("i_");
    return _names;
  }

  SortRegistry() {}

  thread_local static SortRegistry *_instance;

};

} //end of namespace

#endif
