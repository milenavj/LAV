//===-- LState.h ----------------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LSTATE_H
#define LSTATE_H

#include <ostream>
#include <string>
#include <vector>

#include "expression/expressions/Expression.h"
 
#include "lav/Internal/LConstraints.h"
#include "lav/Internal/LBlockStore.h"
#include "lav/Internal/LTypes.h"

namespace llvm
{ 
  struct BasicBlock;
  class ConstantExpr;
  class AllocaInst;
  class Type;
  class Function;
  class Constant;
  class Instruction;
}

namespace lav
{
  class LInstruction;
  class LFunction;
  class LBlock;
  
  class LState
  {
    public:
      
      LState()          : _ParentBlock(NULL) {}
      LState(LBlock* b) : _ParentBlock(b)    {}
      
      inline       LBlockStore& GetStore()       {return _Store;}            
      inline const LBlockStore& GetStore() const {return _Store;}                  

      inline void AddConstraint     (const aExp& e)     {_Constraints.Add(e);} 
      inline void clear             ()                  {_Constraints.clear();}
      inline void ExtractRelevant   ()                  {_Store.ExtractRelevant();} 
      inline void PrintConstraints  ()            const { _Constraints.Print(); }
      inline const LConstraints& GetConstraints() const {return _Constraints;}
      inline aExp  GetStateConstraint          () const {return _Constraints.Constraint();}

      const aExp& GetValue          (const aExp& e); 
      const aExp* GetValue          (const std::string &name);
      mspaExp GetModelValues        () const;

      aExp Constraints              () const;
      aExp TransformationFormula    () const;
      aExp ConnectVariables         (unsigned id1, unsigned id2) const;
      void Update                   (LInstruction* fi);
      void ResetMemory              ();
      void AddMainArgumentConditions();
      void AddGlobalInitializations ();
      void AddStore(const std::string& variable, const LVariableInfo& fvi);

    private:
      aExp     argument             (const llvm::Instruction* i, unsigned int n);        
      aExp     evalConstant         (llvm::Constant *c);
      aExp     evalConstantExpr     (llvm::ConstantExpr *ce);        
      aExp     GetGlobalValue       (const std::string& name);
      aExp     StoreValue           (LInstruction* fi);            
      aExp     GetDivValue          (llvm::Instruction* i, bool b);
//    aExp     GetRemValue          (llvm::Instruction* i, bool b);

      void     ProcessGEPI          (LInstruction* fi);
      void     ProcessReturn        (LInstruction* fi);
      void     ProcessFunctionCall  (LInstruction* fi);
      void     ProcessXor           (LInstruction* fi);
      void     ProcessLoad          (LInstruction* fi);
      void     ProcessStore         (LInstruction* fi);
      void     ProcessBitCast       (LInstruction* fi);
      void     ProcessSwitch        (LInstruction* fi);
      void     ProcessBr            (LInstruction* fi);
      void     ProcessAllocation    (LInstruction* fi);
      void     ProcessICmp          (LInstruction* fi);
      void     ProcessFCmp          (LInstruction* fi);
      void     ProcessPHI           (LInstruction* fi);
      void     ProcessSelect        (LInstruction* fi);
      void     ProcessPtrToInt      (LInstruction* fi);
      void     ProcessIntToPtr      (LInstruction* fi);
      void     ProcessMemcpy        (LInstruction* fi, llvm::Function* f, unsigned numArgs);
      void     ProcessFChange       (LInstruction* fi, llvm::Function* f, unsigned numArgs);
      void     ProcessFInc          (LInstruction* fi, llvm::Function* f, unsigned numArgs);
      void     ProcessFDec          (LInstruction* fi, llvm::Function* f, unsigned numArgs);
      void     ProcessFEnd          (LInstruction* fi, llvm::Function* f, unsigned numArgs);
      void     ProcessLibraryCall   (LInstruction* fi, llvm::Function* f, unsigned numArgs);
      void     Write(const std::string &name, const LVariableInfo& info);
      void     Write(const std::string &name, aExp* Value, const llvm::Type* type, bool rel = false);

      void     WriteIntoStore(llvm::Instruction* i, const aExp& e);
      void     WriteAllocation(LConstraints& constraints, llvm::AllocaInst *ai, unsigned left, unsigned right);           
      void     GepiArgument(const aExp& arg, LInstruction* fi);
      void     StoreGlobalValue(const std::string& name, aExp& value, llvm::Instruction* i);
      void     MemoryStore  (LInstruction* fi, const aExp& index );
      void     MemoryStore  (const aExp& mem, const aExp& index, const aExp& value);
      
      void     InlineFunction                    (LInstruction* fi, llvm::Function* f, unsigned numArg);
      void     ConnectFunctionArguments          (llvm::Instruction* i, llvm::Function* f, unsigned numArgs, LFunction* ff);
      void     ConnectFunctionMemory             (llvm::Function* f, LFunction* ff);
      void     ConnectFunctionReturnValue        (llvm::Function* f, LFunction* ff, llvm::Instruction* i);
      void     ConnectGlobalVariablesEnd         (llvm::Function* f, LFunction* ff);
      void     ConnectGlobalVariablesBegin       (llvm::Function* f, LFunction* ff);
      void     ConnectFunctionConditions         (LInstruction* fi, LFunction* ff);

      inline LBlock*  GetParentBlock() const     {return _ParentBlock;}
      inline std::string GetNameOfNextVariable() const ;
      
      LBlockStore                 _Store;
      LConstraints                _Constraints;
      std::vector<aExp>           _Memory;
      LBlock*                     _ParentBlock;
      
  };

}

#endif

