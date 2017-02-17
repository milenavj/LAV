#include "solvers/solver-interfaces/FormulaFactory.h"
#include <cstdlib>

namespace UrsaMajor {

////////////////////////////////////////////////////////////////////////////////
////	Class FormulaFactory
////////////////////////////////////////////////////////////////////////////////

thread_local FormulaFactory *FormulaFactory::formulaFactory = 0;

// ----------------------------------------------------------------------------

Formula *FormulaFactory::Get(Formula *f) {

  FormulaPointerSet::const_iterator i = existingFormulas.find(f);
  if (i != existingFormulas.end()) {
    if (*i != f) {
      Formula::DEL++;
      delete f;
    }
    return *i;
  } else {
    existingFormulas.insert(f);
    return f;
  }
}

// ----------------------------------------------------------------------------

void FormulaFactory::Remove(Formula *f) {
  //		assert(f != 0);
  if (f->GetRefCount())
    return;
  //		assert(existingFormulas.find(f) != existingFormulas.end());
  if (existingFormulas.find(f) != existingFormulas.end())
    existingFormulas.erase(f);

  delete f;
  Formula::DEL++;
}

// ----------------------------------------------------------------------------

void FormulaFactory::SetIds(unsigned *varCount) {

  FormulaPointerSet::iterator b = begin(), e = end(), iter;
  for (iter = b; iter != e; iter++)
    if ((*iter)->GetType() == VAR)
      (*iter)->SetId(((FormulaVar *)(*iter))->GetOrdinal());
    else if ((*iter)->GetType() != CONST)
      (*iter)->SetId(++(*varCount));
}

// ----------------------------------------------------------------------------

// f is either T or NT or does not includes nor T neither NT
bool FormulaFactory::to_cnf(FormulaVector constraint,
                            vector<vector<int> > &conj,
                            vector<unsigned int> &MappedVarId,
                            unsigned BasicVarCount, unsigned *varCount) {

  vector<vector<int> > conj_tmp;

  vector<bool> NeededFormulas;
  NeededFormulas.resize(*varCount + 1);
  for (unsigned i = 1; i <= BasicVarCount; i++)
    NeededFormulas[i] = true;
  for (unsigned i = BasicVarCount + 1; i <= *varCount; i++)
    NeededFormulas[i] = false;

  assert(constraint.getWidth() == 1);

  Formula *root;
  if ((constraint[0])->GetType() == CONST) {
    if (((FormulaConst *)(constraint[0]))->GetValue() == false)
      return false;
    else
      return true;
  } else {
    vector<int> disj;
    disj.push_back((constraint[0])->GetId());
    root = constraint[0];
    if (root->GetType() != AND) {
      conj_tmp.push_back(disj);
      NeededFormulas[(constraint[0])->GetId()] = true;
    }
  }

  FormulaPointerSet::iterator bf = FormulaFactory::Instance()->begin(),
                              ef = FormulaFactory::Instance()->end(), it;
  for (it = bf; it != ef; it++) {
    Formula *f = *it;

    switch (f->GetType()) {

    case NOT: {
      Formula *tmp = ((FormulaNot *)f)->GetF();
      NeededFormulas[tmp->GetId()] = true;
    } break;

    case AND:
      SetDeepArgsNeeded(f, AND, NeededFormulas);
      break;

    case OR:
      SetDeepArgsNeeded(f, OR, NeededFormulas);
      break;

    case XOR: {
      Formula *l = ((FormulaXor *)f)->GetLeftF(),
              *r = ((FormulaXor *)f)->GetRightF();
      NeededFormulas[l->GetId()] = true;
      NeededFormulas[r->GetId()] = true;
      break;
    }

    case EQUIV: {
      Formula *l = ((FormulaEquiv *)f)->GetLeftF(),
              *r = ((FormulaXor *)f)->GetRightF();
      NeededFormulas[l->GetId()] = true;
      NeededFormulas[r->GetId()] = true;
      break;
    }
    default:
      break;
    }
  }

  vector<int> disj, args;
  for (it = bf; it != ef; it++) {
    Formula *f = *it;

    if (f->GetType() == CONST)
      continue;

    if (!(NeededFormulas[f->GetId()]) && (f != root)) {
      // cout << "formula " << f->GetId() << " not needed" << endl;
      continue;
    }

    switch (f->GetType()) {

    case NOT: {
      Formula *tmp = ((FormulaNot *)f)->GetF();
      disj.push_back(f->GetId());
      disj.push_back(tmp->GetId());
      conj_tmp.push_back(disj);
      disj.clear();

      disj.push_back(-f->GetId());
      disj.push_back(-tmp->GetId());
      conj_tmp.push_back(disj);
      disj.clear();
    } break;

    case AND: {
      GatherDeepArgs(f, AND, args);
      if (f != root) {

        disj.push_back(f->GetId());
        for (size_t i = 0; i < args.size(); i++)
          disj.push_back(-args[i]);
        conj_tmp.push_back(disj);
        disj.clear();

        for (size_t i = 0; i < args.size(); i++) {
          disj.push_back(-f->GetId());
          disj.push_back(args[i]);
          conj_tmp.push_back(disj);
          disj.clear();
        }
        args.clear();
      } else {
        for (size_t i = 0; i < args.size(); i++) {
          disj.push_back(args[i]);
          conj_tmp.push_back(disj);
          disj.clear();
        }
        args.clear();
      }

    } break;

    case OR: {
      GatherDeepArgs(f, OR, args);
      disj.push_back(-f->GetId());
      for (size_t i = 0; i < args.size(); i++)
        disj.push_back(args[i]);
      conj_tmp.push_back(disj);
      disj.clear();

      for (size_t i = 0; i < args.size(); i++) {
        disj.push_back(f->GetId());
        disj.push_back(-args[i]);
        conj_tmp.push_back(disj);
        disj.clear();
      }
      args.clear();

    } break;

    case XOR: {
      Formula *l = ((FormulaXor *)f)->GetLeftF(),
              *r = ((FormulaXor *)f)->GetRightF();
      disj.push_back(-f->GetId());
      disj.push_back(l->GetId());
      disj.push_back(r->GetId());
      conj_tmp.push_back(disj);
      disj.clear();

      disj.push_back(-f->GetId());
      disj.push_back(-l->GetId());
      disj.push_back(-r->GetId());
      conj_tmp.push_back(disj);
      disj.clear();

      disj.push_back(f->GetId());
      disj.push_back(-l->GetId());
      disj.push_back(r->GetId());
      conj_tmp.push_back(disj);
      disj.clear();

      disj.push_back(f->GetId());
      disj.push_back(l->GetId());
      disj.push_back(-r->GetId());
      conj_tmp.push_back(disj);
      disj.clear();
    } break;

    case EQUIV: {
      Formula *l = ((FormulaXor *)f)->GetLeftF(),
              *r = ((FormulaXor *)f)->GetRightF();
      disj.push_back(-f->GetId());
      disj.push_back(-l->GetId());
      disj.push_back(r->GetId());
      conj_tmp.push_back(disj);
      disj.clear();

      disj.push_back(-f->GetId());
      disj.push_back(l->GetId());
      disj.push_back(-r->GetId());
      conj_tmp.push_back(disj);
      disj.clear();

      disj.push_back(f->GetId());
      disj.push_back(l->GetId());
      disj.push_back(r->GetId());
      conj_tmp.push_back(disj);
      disj.clear();

      disj.push_back(f->GetId());
      disj.push_back(-l->GetId());
      disj.push_back(-r->GetId());
      conj_tmp.push_back(disj);
      disj.clear();
    } break;

    default:
      break;
    }
  }

  // cout << "tmp: p cnf " << *varCount << " " << conj_tmp.size() << " " <<
  // endl;
  unsigned int ReducedVars = 0;
  for (unsigned int i = 1; i <= *varCount; i++) {
    if (NeededFormulas[i])
      MappedVarId[i] = ++ReducedVars;
    else
      MappedVarId[i] = 0;
    // cout << "var " << i << " -> " << ReducedVars << endl;
  }
  *varCount = ReducedVars;

  vector<vector<int> >::iterator b_c = conj_tmp.begin(), e_c = conj_tmp.end();
  vector<int>::iterator b_d, e_d;
  vector<int> clause;
  int mappedVar;
  for (vector<vector<int> >::iterator iter_c = b_c; iter_c != e_c; iter_c++) {
    b_d = iter_c->begin();
    e_d = iter_c->end();

    if (NeededFormulas[abs(*b_d)]) {
      for (vector<int>::iterator iter_d = b_d; iter_d != e_d; iter_d++) {
        mappedVar =
            (*iter_d > 0 ? MappedVarId[*iter_d] : -MappedVarId[-(*iter_d)]);
        // cout << "( " << *iter_d << "/" << mappedVar << ")" ;
        clause.push_back(mappedVar);
      }
      // cout << endl;
      conj.push_back(clause);
      clause.clear();
    } else
      throw "Warning! Formula unexpectedly not needed!";
  }

  return true;
}

// ----------------------------------------------------------------------------

void FormulaFactory::SetDeepArgsNeeded(Formula *f, FORMULA_TYPE type,
                                       vector<bool> &neededFormulas) {
  if (f->GetType() == type) {
    SetDeepArgsNeeded(((FormulaOr *)f)->GetLeftF(), type, neededFormulas);
    SetDeepArgsNeeded(((FormulaOr *)f)->GetRightF(), type, neededFormulas);
  } else {
    if (f->GetType() == NOT) {
      Formula *n = ((FormulaNot *)f)->GetF();
      neededFormulas[n->GetId()] = true;
    } else {
      neededFormulas[f->GetId()] = true;
      //    cout << "formula " << f->GetId() << "is needed" << endl;
    }
  }
}

// ----------------------------------------------------------------------------

void FormulaFactory::GatherDeepArgs(Formula *f, FORMULA_TYPE type,
                                    vector<int> &args) {
  if (f->GetType() == type) {
    GatherDeepArgs(((FormulaOr *)f)->GetLeftF(), type, args);
    GatherDeepArgs(((FormulaOr *)f)->GetRightF(), type, args);
  } else {
    if (f->GetType() == NOT) {
      Formula *n = ((FormulaNot *)f)->GetF();
      args.push_back(-n->GetId());
    } else {
      args.push_back(f->GetId());
    }
  }
}

// ----------------------------------------------------------------------------

} //end of namespace
