//===-- LBlockStore.h -----------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef FBLOCKSTORE_H
#define FBLOCKSTORE_H

#include <map>
#include <string>
#include <vector>



#include "expression/expressions/Expression.h"
#include "lav/Internal/LTypes.h"

namespace llvm
{ 
  class Type;
}
  
 
namespace lav
{

class LVariableInfo
{
  public:
    LVariableInfo   () : _Value(NULL), _Type(NULL), _Relevant(false), _Connect(true), _TransformInclude(true) {}
    LVariableInfo   (aExp* v, const llvm::Type* t, bool rel = false, bool conn = true, bool tt = true);         
    LVariableInfo(const LVariableInfo& fv)                           { init(fv); }
    LVariableInfo& operator=(const LVariableInfo& fv);
    ~LVariableInfo  ()                                               { deinit(); }

    inline void                    SetRelevant     (bool r)                 { _Relevant = r;}
    inline void                    SetConnect      (bool r)                 { _Connect  = r;}
    inline void                    SetTransformInclude(bool r)              { _TransformInclude  = r;}
    inline const llvm::Type*&      Type            ()                       { return _Type; }
    inline aExp*&                   Value           ()                       { return _Value; }
    inline void                    DeleteValue     ()                       { delete _Value;}
    inline void                    DeleteType      ()                       { _Type = NULL;}

    inline bool                    IsRelevant      ()              const    { return _Relevant;}
    inline bool                    Connect         ()              const    { return _Connect;}
    inline bool                    IncludeInTransformation ()      const    { return _TransformInclude;}
    inline const llvm::Type*       Type            ()              const    { return _Type; }
    inline const aExp*             Value           ()              const    { return _Value; }

    std::ostream&                  Print           (ostream& ostr) const;

  private:
    void                    deinit();
    void                    init(const LVariableInfo& fv);

  private:
    aExp*       _Value;
    const llvm::Type*       _Type;
    bool                    _Relevant;
    bool                    _Connect;
    bool                    _TransformInclude;
};

class LBlockStore
{
  public:
        
    const std::map<std::string, LVariableInfo>& GetStore() const
    {return _Store;}

    const aExp* GetValue(cStr &name);
    const llvm::Type* GetType(cStr &name) const;

    void ChangeType(cStr &name, const llvm::Type* t);
    void ChangeValue(cStr &name, aExp* Value);

    void Write(cStr &name, const LVariableInfo& info);
    void Write(cStr &name, aExp* Value, const llvm::Type* type, bool rel = false);

    void ChangeRelevant(cStr &name, bool rel);
    void ChangeConnect (cStr &name, bool rel);
    void ChangeTransform (cStr &name, bool rel);

    bool GetConnect (cStr &name) ;
    bool IsDefined(cStr &name) const;
    std::ostream& Print(ostream& ostr) const;
    void ExtractRelevant();


  private:
    std::map<std::string, LVariableInfo>    _Store;
};


}//end of namespace

#endif
