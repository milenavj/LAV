//===-- InlineAll.cpp
//-----------------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "inline"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Support/CallSite.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/IPO/InlinerPass.h"
#include "llvm/Analysis/InlineCost.h"
#include "llvm/ADT/SmallPtrSet.h"

using namespace llvm;

namespace {

// AllInliner only inlines all functions .
//  class VISIBILITY_HIDDEN AllInliner : public Inliner {
class AllInliner : public Inliner {
  // Functions that are never inlined
  SmallPtrSet<const Function *, 16> NeverInline;
  llvm::InlineCost CA;
  static const int tresh = 1000000000;

public:
  // Use extremely high threshold.
  //fixmeT true ili false???
  AllInliner(int Threshold)
      : Inliner(ID, Threshold, true), CA(InlineCost::get(100, Threshold)) {}

  // Use extremely high threshold.
  AllInliner() : Inliner(ID, tresh, true), CA(InlineCost::get(100, tresh)) {}
  static char ID; // Pass identification, replacement for typeid

  //    InlineCost getInlineCost(CallSite CS) {
  //      return CA.getInlineCost(CS, NeverInline);
  //}

  //fixmeT
  InlineCost getInlineCost(CallSite CS) {
    return InlineCost::get(CA.getCost(), getInlineThreshold(CS));
  }

  //fixmeT
  //    float getInlineFudgeFactor(CallSite CS) {
  //      return CA.getInlineFudgeFactor(CS);
  //    }

  //fixmeT
  void resetCachedCostInfo(Function *Caller) {
    //      return
    //		CA.resetCachedCostInfo(Caller);
  }
  //fixmeT isto je hidden
  /*    virtual bool doFinalization(CallGraph &CG) {
        return removeDeadFunctions(CG, &NeverInline);
      }*/
  //fixmeT /home/novi/llvm/llvm-3.3.src/include/llvm/Pass.h:110:16: warning:
  //‘virtual bool llvm::Pass::doInitialization(llvm::Module&)’ was hidden
  //[-Woverloaded-virtual]

  //    virtual bool doInitialization(CallGraph &CG);
};
}

char AllInliner::ID = 0;
static RegisterPass<AllInliner> X("all-inline", "Inliner for functions");

namespace lav {
Pass *createAllInlinerPass() { return new AllInliner(); }
Pass *createAllInlinerPass(int Threshold) { return new AllInliner(Threshold); }
}

// doInitialization - Initializes the vector of functions that have not
// been annotated with the "always inline" attribute.
/*bool AllInliner::doInitialization(CallGraph &CG) {
  Module &M = CG.getModule();

  for (Module::iterator I = M.begin(), E = M.end();
       I != E; ++I)
    if (!I->isDeclaration() && I->hasFnAttribute(Attribute::NoInline))
      NeverInline.insert(I);

  return false;
}*/
