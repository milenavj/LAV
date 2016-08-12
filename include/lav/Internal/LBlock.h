//===-- LBlock.h ----------------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LBLOCK_H
#define LBLOCK_H

#include <ostream>
#include <string>
#include <vector>
#include <set>

#include "expression/expressions/Expression.h"

#include "lav/Internal/LTypes.h"
#include "lav/Internal/LBlockStore.h"
#include "lav/Internal/LLocalCondition.h"
#include "lav/Internal/LJump.h"
#include "lav/Internal/LState.h"
#include "lav/Internal/LMerge.h"

namespace llvm {
struct BasicBlock;
class Type;
}

namespace lav {
class LFunction;
class LModule;
class LInstruction;

class LBlock {

public:

  LBlock(llvm::BasicBlock *bb, LFunction *parent);
  ~LBlock();

  void CalculateDescriptions();
  void CalculateConditions();
  void CalculateConditionsIncremental();
  void CalculateConditionsBlock();
  void ReCalculateConditions(int n);
  void RecalculatePostcondition();

  //dodavanje lokalnih uslova
  void AddLocalCondition(const aExp &e, LInstruction *fi, ERRKIND er);
  void AddLocalConditionZeroDisequality(const aExp &e, LInstruction *fi);
  void AddLocalConditionPointers(const aExp &e, LInstruction *fi);
  void ConnectFunctionConditions(LInstruction *fi, LFunction *ff);
  void AddStore(const vStr &variables, const vVarInfo &fvis);

  inline const vpBlock &GetPreds() const { return _Preds; }
  inline const vJump &GetJumps() const { return _Jumps; }
  inline unsigned JumpsNumber() const { return _Jumps.size(); }
  inline unsigned PredsNumber() const { return _Preds.size(); }
  inline void AddJump(const LJump &jump) { _Jumps.push_back(jump); }
  inline void AddNoPred(LBlock *fb) { _NoPreds.push_back(fb); }
  inline void AddNoJump(LBlock *fb) { _NoPreds.push_back(fb); }
  LBlock *GetPred() const; //ovo je cudno
  bool IsInsideLoop() const;

  aExp Postcondition();
  inline bool PostconditionInSolver() const { return _PostconditionInSolver; }
  inline bool &PostconditionInSolver() { return _PostconditionInSolver; }
  inline bool PostconditionIsSet() const { return _PostconditionIsSet; }
  inline const vLocCond &GetLocalConditions() const { return _LocalConditions; }
  inline llvm::BasicBlock *GetBasicBlock() const { return _BBlock; }
  inline bool StateIsSet() const { return _StateIsSet; }
  inline unsigned Id() const { return _Id; }
  inline bool HasNoReturnFunctionCall() const { return _HasNoReturnFC; }
  inline void SetNoReturnFunctionCall() { _HasNoReturnFC = true; }
  inline const LState &GetState() const { return _State; }
  inline LState &GetState() { return _State; }
  inline LFunction *GetParentFunction() const { return _Parent; }
  const std::string &GetFunctionName() const;
  const LModule *GetParentModule() const;
  inline static void ResetBlockNumber() { BlockNumber = 0; }
  int GetContext() const;
  const aExp &GetTrace();
  aExp GetTraceGlobFuncCons();
  aExp GetGlobFuncCons() const;
  aExp Active() const;
  bool IsEntryBlock() const;

  std::ostream &Print(std::ostream &ostr) const;
  std::ostream &PrintHTML(std::ostream &ostr) const;

  bool IsUnreachableBlock() const;
  void ChangeInitStore();
  void SetState();

  vUnsigned GetLastLineStack() const;
  const LBlock *GetPredWithId(unsigned id) const;

  mspaExp GetModelValues() const { return _State.GetModelValues(); }
  const mspType &GetReferences() const;

  //spajanje blokova
  void TryMerge();
  inline bool IsMerged() const { return _MergeInfo.IsMerged(); }
  inline LBlock *MergedInto() const { return _MergeInfo.MergedInto(); }
  inline bool HasMerged() const { return _MergeInfo.HasMerged(); }
  inline const vpBlock &BlocksMerged() const {
    return _MergeInfo.BlocksMerged();
  }

  aExp AddAddresses(const aExp &e);
  void UpdateAndSetAddresses();

private:

  void init();
  void SetJumps();
  void SetPreds();
  void SetPostcondition();
  void SetPostconditionTop();

  bool QuickCalculate();
  aExp MakeTrace() const;
  bool SkipLocalCondition(LLocalCondition &lc);
  bool CheckReachability();

  //ovo je za rad sa petljama
  bool DoCalculate() const;
  bool DoNotCalculate() const;
  void AddToLoopMax() const;

  inline const vLoop &Loop() const { return _Loop; }

  void FlawedFound(const LInstruction *fi, ERRKIND er);
  bool ProcessStatus(const LInstruction *fi, ERRKIND er, STATUS s);
  void stopWhenFound(const LInstruction *fi, STATUS s, bool count);

  aExp GetExitConditions() const;
  aExp GetEntryConditions() const;
  aExp GetPredsConditions() const;
  void AddNewPreds(vpBlock &preds, LBlock *p) const;
  aExp BlockEntry() const;
  void UpdateAddresses();
  aExp Addresses() const { return _Addresses; }

  aExp AllConstraints() const;
  aExp ConnectBlocks(unsigned id1, unsigned id2) const;
  void AddPredsConditionsIncremental();

  bool HasAssume() const;
  void CalculateAssume();

private:

  LBlock(const LBlock &fb);
  LBlock &operator=(const LBlock &fb);

  static unsigned BlockNumber;

  unsigned _Id;
  llvm::BasicBlock *_BBlock;
  vpInst _Instructions;
  LFunction *_Parent;

  LState _State;
  aExp _Postcondition;
  aExp _Addresses;
  vLocCond _LocalConditions;

  bool _PredsSet;
  bool _TraceCalculated;
  bool _DescriptionsCalculated;
  bool _ConditionsCalculated;
  bool _StateIsSet;
  bool _PostconditionIsSet;
  bool _PostconditionInSolver;
  bool _ChangeInitStore;

  aExp _Trace;
  vJump _Jumps;
  vpBlock _Preds;
  vpBlock _NoPreds;
  vpBlock _NoJumps;

  bool _HasNoReturnFC;
  bool _HasAssume;
  LMerge _MergeInfo;

  enum BLOCK_REACHABILITY {
    BLOCK_UNCHECKED,
    BLOCK_REACHABLE,
    BLOCK_UNREACHABLE
  };
  enum BLOCK_REACHABILITY _Reachable;

  vLoop _Loop;

  //!@#$
  bool _Desc;
  bool _RecDetect;
};

} //end of namespace

#endif
