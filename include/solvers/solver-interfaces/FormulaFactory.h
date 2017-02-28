#ifndef __FORMULA_FACTORY_H
#define __FORMULA_FACTORY_H

#include <assert.h>

#include <tr1/unordered_set>
using std::tr1::unordered_set;

#include <vector>
#include <set>
using namespace std;

#include "FormulaVector.h"

////////////////////////////////////////////////////////////////////////////////
////	Class FormulaFactory
////////////////////////////////////////////////////////////////////////////////
namespace UrsaMajor {

class FormulaFactory {

private:

  FormulaFactory() {}

  class equal_to_Formula {
  public:
    bool operator()(const Formula *f1, const Formula *f2) const {
      return (*f1) == (*f2);
    }
  };

  class hash_Formula {
  public:
    size_t operator()(const Formula *f) const { return f->hashCode(); }
  };

  typedef unordered_set<Formula *, hash_Formula, equal_to_Formula>
      FormulaPointerSet;
  FormulaPointerSet existingFormulas;
  thread_local static FormulaFactory *formulaFactory;

public:

  static FormulaFactory *Instance() {
    if (FormulaFactory::formulaFactory == 0)
      FormulaFactory::formulaFactory = new FormulaFactory();
    return FormulaFactory::formulaFactory;
  }

  ~FormulaFactory() {
    Clear();
    //		existingFormulas.clear();
    //		formulaFactory = 0;
  }

  void Clear() {
    existingFormulas.clear();
    formulaFactory = 0;
  }

  FormulaPointerSet::iterator begin(void) { return existingFormulas.begin(); }
  FormulaPointerSet::iterator end(void) { return existingFormulas.end(); }

  Formula *Get(Formula *f);
  void Remove(Formula *f);
  void SetIds(unsigned *varCount);
  bool to_cnf(FormulaVector constraint, vector<vector<int> > &conj,
              vector<unsigned int> &MappedVarId, unsigned BasicVarCount,
              unsigned *varCount);

private:
  void SetDeepArgsNeeded(Formula *f, FORMULA_TYPE type,
                         vector<bool> &neededFormulas);
  void GatherDeepArgs(Formula *f, FORMULA_TYPE type, vector<int> &args);

};

} //end of namespace
#endif
