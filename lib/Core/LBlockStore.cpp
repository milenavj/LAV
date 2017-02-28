//===-- LBlockStore.cpp
//----------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "lav/Internal/LBlockStore.h"
#include "lav/Internal/LCommon.h"

#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/raw_ostream.h"

#include "expression/output/SMTFormater.h"

namespace lav {
static argo::SMTFormater SMTF;

//////////////////////////////////////////////////////////////
// LVariableInfo
//////////////////////////////////////////////////////////////

LVariableInfo::LVariableInfo(argo::Expression *v, const llvm::Type *t, bool rel,
                             bool conn, bool b)
    : _Value(v), _Type(t), _Relevant(rel), _Connect(conn),
      _TransformInclude(b) {
  if (v != NULL) {
    _Value = new argo::Expression(SimplifyExpression(*v));
    delete v;
  }
}

LVariableInfo &LVariableInfo::operator=(const LVariableInfo &fv) {
  if (&fv != this) {
    deinit();
    init(fv);
  }
  return *this;
}

std::ostream &LVariableInfo::Print(ostream &ostr) const {
  if (_Relevant)
    ostr << "relevant ";
  else
    ostr << "irelevant ";
  if (_Connect)
    ostr << "connect ";
  else
    ostr << "disconnect ";

  if (_Type != NULL) {
    std::string s;
    llvm::raw_string_ostream ros(s);
    _Type->print(ros);
    ostr << ros.str() << ' ';
  } else
    ostr << "(null) ";

  if (_Value != NULL) {
    _Value->Print(&SMTF, ostr);
  } else
    ostr << "(null) ";
  return ostr;
}

void LVariableInfo::init(const LVariableInfo &fv) {
  if (fv.Value() == NULL)
    _Value = NULL;
  else
    _Value = new argo::Expression(*fv.Value());

  _Type = fv.Type();
  _Relevant = fv.IsRelevant();
  _Connect = fv.Connect();
  _TransformInclude = fv.IncludeInTransformation();
}

void LVariableInfo::deinit() { delete _Value; }

//////////////////////////////////////////////////////////////
// LBlockStore
//////////////////////////////////////////////////////////////
void LBlockStore::ExtractRelevant() {
  std::map<std::string, LVariableInfo>::iterator iter = _Store.begin();
  std::vector<std::map<std::string, LVariableInfo>::iterator> todelete;

  for (; iter != _Store.end(); iter++)
    if (!iter->second.IsRelevant())
      todelete.push_back(iter);

  for (unsigned i = 0; i < todelete.size(); i++)
    _Store.erase(todelete[i]);
}

void LBlockStore::Write(const std::string &name, const LVariableInfo &info) {
  if (!IsDefined(name))
    _Store[name] = info;
  else {
    assert(info.IsRelevant() == _Store[name].IsRelevant());
    bool r = _Store[name].IsRelevant();
    LVariableInfo f(info);
    f.SetRelevant(r);
    _Store[name] = f;
  }
}

void LBlockStore::Write(const std::string &name, argo::Expression *Value,
                        const llvm::Type *type, bool rel) {
  if (!IsDefined(name))
    _Store[name] = LVariableInfo(Value, type, rel);
  else {
    bool r = _Store[name].IsRelevant();
    LVariableInfo f(Value, type, r);
    _Store[name] = f;
  }
}

const argo::Expression *LBlockStore::GetValue(const std::string &name) {

  std::map<std::string, LVariableInfo>::const_iterator iter = _Store.find(name);
  if (iter != _Store.end())
    return iter->second.Value();
  else
    return NULL;
}

const llvm::Type *LBlockStore::GetType(const std::string &name) const {
  std::map<std::string, LVariableInfo>::const_iterator iter = _Store.find(name);
  assert(iter != _Store.end() && "store is not defined for this name!");
  return iter->second.Type();
}

void LBlockStore::ChangeValue(const std::string &name,
                              argo::Expression *Value) {
  assert(IsDefined(name));
  assert(_Store[name].Value() != Value);

  if (_Store[name].Value() != Value) {
    _Store[name].DeleteValue();
    argo::Expression e = SimplifyExpression(*Value);
    delete Value;
    _Store[name].Value() = new argo::Expression(e);
  }
}

void LBlockStore::ChangeType(const std::string &name, const llvm::Type *t) {
  assert(IsDefined(name));
  _Store[name].DeleteType();
  _Store[name].Type() = t;
}

void LBlockStore::ChangeRelevant(const std::string &name, bool rel) {
  assert(IsDefined(name));
  _Store[name].SetRelevant(rel);
}

void LBlockStore::ChangeConnect(const std::string &name, bool rel) {
  assert(IsDefined(name));
  _Store[name].SetConnect(rel);
}

bool LBlockStore::GetConnect(const std::string &name) {
  assert(IsDefined(name));
  return _Store[name].Connect();
}

void LBlockStore::ChangeTransform(const std::string &name, bool rel) {
  assert(IsDefined(name));
  _Store[name].SetTransformInclude(rel);
}

bool LBlockStore::IsDefined(const std::string &name) const {
  std::map<std::string, LVariableInfo>::const_iterator iter = _Store.find(name);
  if (iter != _Store.end()) // && (iter->second.Value() != null))
    return true;
  else
    return false;
}

std::ostream &LBlockStore::Print(ostream &ostr) const {
  if (_Store.size() > 0) {
    std::map<std::string, LVariableInfo>::const_iterator iter = _Store.begin();
    ostr << "------------------------- " << std::endl;
    for (; iter != _Store.end(); iter++) {
      ostr << "[ " << iter->first << " ";
      iter->second.Print(ostr);
      ostr << " ]" << std::endl;
    }
    ostr << "------------------------- " << std::endl;
  }
  return ostr;
}

} //end of namespace
