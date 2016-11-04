//===-- main.cpp ----------------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "config.h"
#include "lav/Internal/LCommon.h"
#include "lav/Transformation/LLoop.h"
#include "lav/Transformation/LSimplify.h"
#include "lav/Internal/LLav.h"
#include "lav/Misc/misc.h"

#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/PassNameParser.h"
#include "llvm/Support/Timer.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/system_error.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/PassManager.h"
#include "llvm/Analysis/Verifier.h"

#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"

#include <iostream>
#include <string>

using namespace llvm;
using namespace lav;

extern llvm::cl::opt<std::string> InputFile;
extern llvm::cl::opt<std::string> OutputFolder;

namespace {
llvm::cl::opt<bool> EnableOptimizations(
    "enable-optimizations",
    llvm::cl::desc(
        "LAV --- Enable LLVM optimizations (default = false) --- experimental"),
    llvm::cl::init(false));

llvm::cl::opt<int>
    TimeOut("timeout", llvm::cl::desc("LAV --- Kill LAV if it reaches timeout "
                                      "(default = 0s, i.e. no timeout)"),
            llvm::cl::init(0));
}

void CheckSolver();
static void AddTransformPasses(PassManager &PM);
static inline void addPass(PassManager &PM, Pass *P);
static void CheckTimeOut(int argc, char **argv);
int main(int argc, char **argv) {

  atexit(llvm_shutdown); // Call llvm_shutdown() on exit.
                         //  llvm::InitializeNativeTarget();
  lav::parseArguments(argc, argv);
  sys::PrintStackTraceOnErrorSignal();

  CheckSolver();
  CheckTimeOut(argc, argv);

  // Load the bytecode...
//  Timer LoadingTime("Loading the bytecode");
  std::cout << std::endl << "Loading the bytecode..." << std::endl;
//  LoadingTime.startTimer();
  std::string ErrorMsg;

  Module *mainModule = 0;

  OwningPtr<MemoryBuffer> BufferPtr;
  llvm::error_code ec =
      MemoryBuffer::getFileOrSTDIN(InputFile.c_str(), BufferPtr);
  if (ec) {
    lav::exit_error((std::string) "error loading program " + InputFile.c_str() +
                    " : " + ec.message().c_str());
  }
  mainModule =
      getLazyBitcodeModule(BufferPtr.get(), getGlobalContext(), &ErrorMsg);

  if (mainModule) {
    if (mainModule->MaterializeAllPermanently(&ErrorMsg)) {
      delete mainModule;
      mainModule = 0;
    }
  }
  if (!mainModule)
    lav::exit_error((std::string) "error loading program '%s': %s" +
                    InputFile.c_str() + ErrorMsg.c_str());
//  LoadingTime.stopTimer();

  std::cout << "Loading the bytecode... Completed " << std::endl << std::endl;

  //  llvm::outs() << *mainModule;

  // Prepare output directory
  MakeDirectory(OutputFolder);

  // Instantiate the pass manager to organize the passes.
  PassManager Passes;

  // Add an appropriate TargetData instance for this module...
  //  addPass(Passes, new TargetData(mainModule));
  // Add an appropriate DataLayout instance for this module...
  addPass(Passes, new DataLayout(mainModule));

  // Add neccessary transformations
  AddTransformPasses(Passes);

  // Add LAV analysis
  //#!
  addPass(Passes, createLavPass());

  //  llvm::outs() << *mainModule;
  // Run our queue of passes all at once now, efficiently.
  Passes.run(*mainModule);

  //  llvm::outs() << *mainModule;

  BufferPtr.take();
  std::cout << "Finished " << std::endl << std::endl;

  return 0;
}

static void CheckTimeOut(int argc, char **argv) {
  if (TimeOut > 0) {
    std::string s = "time timeout --k=1 ";
    s += ItoS(TimeOut);
    s += " ";
    for (int i = 0; i < argc; i++) {
      if (strstr(argv[i], "timeout")) {
        if (i + 1 < argc && atoi(argv[i + 1]) == TimeOut)
          i++;
        continue;
      }
      s += (std::string(" ") + argv[i]);
    }
    std::string echo = (std::string) "echo " + s;
    int sys = system(echo.c_str());
    sys = system(s.c_str());
    exit(sys);
  }

}

bool VerifyEach = false;

// A utility function that adds a pass to the pass manager but will also add
// a verifier pass after if we're supposed to verify.
static inline void addPass(PassManager &PM, Pass *P) {
  // Add the pass to the pass manager...
  PM.add(P);

  // If we are verifying all of the intermediate steps, add the verifier...
  if (VerifyEach)
    PM.add(createVerifierPass());
}

static void AddStandardCompilePasses(PassManager &PM) {
  PM.add(createVerifierPass()); // Verify that input is correct

  //  addPass(PM, createLowerSetJmpPass());          // Lower
  // llvm.setjmp/.longjmp

  // If the -strip-debug command line option was specified, do it.
  //  if (StripDebug)
  //    addPass(PM, createStripSymbolsPass(true));

  if (!EnableOptimizations)
    return;

  //  addPass(PM, createRaiseAllocationsPass());     // call %malloc -> malloc
  // inst
  addPass(PM, createCFGSimplificationPass());       // Clean up disgusting code
  addPass(PM, createPromoteMemoryToRegisterPass()); // Kill useless allocas
  addPass(PM, createGlobalOptimizerPass());         // Optimize out global vars
  addPass(PM, createGlobalDCEPass());             // Remove unused fns and globs
  addPass(PM, createIPConstantPropagationPass()); // IP Constant Propagation
  addPass(PM, createDeadArgEliminationPass());    // Dead argument elimination
  addPass(PM, createInstructionCombiningPass());  // Clean up after IPCP & DAE
  addPass(PM, createCFGSimplificationPass());     // Clean up after IPCP & DAE

  addPass(PM, createPruneEHPass());       // Remove dead EH info
  addPass(PM, createFunctionAttrsPass()); // Deduce function attrs

  //  if (!DisableInline)
  addPass(PM, createFunctionInliningPass());  // Inline small functions
  addPass(PM, createArgumentPromotionPass()); // Scalarize uninlined fn args

  addPass(PM, createSimplifyLibCallsPass());     // Library Call Optimizations
  addPass(PM, createInstructionCombiningPass()); // Cleanup for scalarrepl.
  addPass(PM, createJumpThreadingPass());        // Thread jumps.
  addPass(PM, createCFGSimplificationPass());    // Merge & remove BBs
  addPass(PM, createScalarReplAggregatesPass()); // Break up aggregate allocas
  addPass(PM, createInstructionCombiningPass()); // Combine silly seq's
  //  addPass(PM, createCondPropagationPass());      // Propagate conditionals

  addPass(PM, createTailCallEliminationPass()); // Eliminate tail calls
  addPass(PM, createCFGSimplificationPass());   // Merge & remove BBs
  addPass(PM, createReassociatePass());         // Reassociate expressions
  addPass(PM, createLoopRotatePass());
  addPass(PM, createLICMPass());         // Hoist loop invariants
  addPass(PM, createLoopUnswitchPass()); // Unswitch loops.
  //  addPass(PM, createLoopIndexSplitPass());       // Index split loops.

  // FIXME : Removing instcombine causes nestedloop regression.
  addPass(PM, createInstructionCombiningPass());

  //  addPass(PM, createLCSSAPass());                //loop closed ssa form -
  // neophodno da bi se uradilo razmotavanje, ja dodala
  addPass(PM, createIndVarSimplifyPass());       // Canonicalize indvars
  addPass(PM, createLoopDeletionPass());         // Delete dead loops
  addPass(PM, createLoopUnrollPass());           // Unroll small loops
  addPass(PM, createInstructionCombiningPass()); // Clean up after the unroller
  addPass(PM, createGVNPass());                  // Remove redundancies
  addPass(PM, createMemCpyOptPass());            // Remove memcpy / form memset
  addPass(PM, createSCCPPass());                 // Constant prop with SCCP

  // Run instcombine after redundancy elimination to exploit opportunities
  // opened up by them.
  addPass(PM, createInstructionCombiningPass());
  //  addPass(PM, createCondPropagationPass());      // Propagate conditionals

  addPass(PM, createDeadStoreEliminationPass()); // Delete dead stores
  addPass(PM, createAggressiveDCEPass());        // Delete dead instructions
  addPass(PM, createCFGSimplificationPass());    // Merge & remove BBs
  addPass(PM, createStripDeadPrototypesPass());  // Get rid of dead prototypes
  //  addPass(PM, createDeadTypeEliminationPass());  // Eliminate dead types
  addPass(PM, createConstantMergePass()); // Merge dup global constants
}

/// Optimize - Perform link time optimizations. This will run the scalar
/// optimizations, any loaded plugin-optimization modules, and then the
/// inter-procedural optimizations if applicable.
void Optimize(PassManager &Passes) {

  // If we're verifying, start off with a verification pass.
  if (VerifyEach)
    Passes.add(createVerifierPass());

  // DWD - Run the opt standard pass list as well.
  AddStandardCompilePasses(Passes);

  if (EnableOptimizations) {
    // Now that composite has been compiled, scan through the module, looking
    // for a main function.  If main is defined, mark all other functions
    // internal.
    //    if (!DisableInternalize)
    //      addPass(Passes, createInternalizePass(true));
    ModulePass *pass =
        createInternalizePass(std::vector<const char *>(1, "main"));
    addPass(Passes, pass);
    // Propagate constants at call sites into the functions they call.  This
    // opens opportunities for globalopt (and inlining) by substituting function
    // pointers passed as arguments to direct uses of functions.
    addPass(Passes, createIPSCCPPass());

    // Now that we internalized some globals, see if we can hack on them!
    addPass(Passes, createGlobalOptimizerPass());

    // Linking modules together can lead to duplicated global constants, only
    // keep one copy of each constant...
    addPass(Passes, createConstantMergePass());

    // Remove unused arguments from functions...
    addPass(Passes, createDeadArgEliminationPass());

    // Reduce the code after globalopt and ipsccp.  Both can open up significant
    // simplification opportunities, and both can propagate functions through
    // function pointers.  When this happens, we often have to resolve varargs
    // calls, etc, so let instcombine do this.
    addPass(Passes, createInstructionCombiningPass());

    //    if (!DisableInline)
    addPass(Passes, createFunctionInliningPass()); // Inline small functions

    addPass(Passes, createPruneEHPass());         // Remove dead EH info
    addPass(Passes, createGlobalOptimizerPass()); // Optimize globals again.
    addPass(Passes, createGlobalDCEPass());       // Remove dead functions

    // If we didn't decide to inline a function, check to see if we can
    // transform it to pass arguments by value instead of by reference.
    addPass(Passes, createArgumentPromotionPass());

    // The IPO passes may leave cruft around.  Clean up after them.
    addPass(Passes, createInstructionCombiningPass());
    addPass(Passes, createJumpThreadingPass());        // Thread jumps.
    addPass(Passes, createScalarReplAggregatesPass()); // Break up allocas

    // Run a few AA driven optimizations here and now, to cleanup the code.
    addPass(Passes, createFunctionAttrsPass()); // Add nocapture
    addPass(Passes, createGlobalsModRefPass()); // IP alias analysis

    addPass(Passes, createLICMPass());                 // Hoist loop invariants
    addPass(Passes, createGVNPass());                  // Remove redundancies
    addPass(Passes, createMemCpyOptPass());            // Remove dead memcpy's
    addPass(Passes, createDeadStoreEliminationPass()); // Nuke dead stores

    // Cleanup and simplify the code after the scalar optimizations.
    addPass(Passes, createInstructionCombiningPass());

    addPass(Passes, createJumpThreadingPass());           // Thread jumps.
    addPass(Passes, createPromoteMemoryToRegisterPass()); // Cleanup jumpthread.

    // Delete basic blocks, which optimization passes may have killed...
    addPass(Passes, createCFGSimplificationPass());

    // Now that we have optimized the program, discard unreachable functions...
    addPass(Passes, createGlobalDCEPass());
  }

  // If the -s or -S command line options were specified, strip the symbols out
  // of the resulting program to make it smaller.  -s and -S are GNU ld options
  // that we are supporting; they alias -strip-all and -strip-debug.
  //  if (Strip || StripDebug)
  //    addPass(Passes, createStripSymbolsPass(StripDebug && !Strip));

  // The user's passes may leave cruft around. Clean up after them them but
  // only if we haven't got DisableOptimizations set
  if (EnableOptimizations) {
    addPass(Passes, createInstructionCombiningPass());
    addPass(Passes, createCFGSimplificationPass());
    addPass(Passes, createAggressiveDCEPass());
    addPass(Passes, createGlobalDCEPass());
  }

  // Make sure everything is still good.
  if (VerifyEach)
    Passes.add(createVerifierPass());

}

static void AddTransformPasses(PassManager &PM) {
  PM.add(createVerifierPass()); // Verify that input is correct

  if (EnableOptimizations) {
    //  Optimize(PM);

    //ovo za pocetak da se proba sta se desava umesto cele optimizacije
    //  addPass(PM, createCFGSimplificationPass());    // Clean up disgusting
    // code
    addPass(PM, createPromoteMemoryToRegisterPass()); // Kill useless allocas
    //  addPass(PM, createCFGSimplificationPass());    // Clean up disgusting
    // code
    //to bi trebalo obrisati i vratiti celu optimizaciju

    addPass(PM, createIPConstantPropagationPass()); // IP Constant Propagation
    addPass(PM, createLoopSimplifyPass());          //Canonicalize natural loops
    addPass(PM, createLoopRotatePass());
    addPass(PM, createLCSSAPass());              //loop closed ssa form
    addPass(PM, createFCFGSimplificationPass()); // lav cfg simplification
    addPass(PM, createFLoopUnrollPass());        // lav unroll
    addPass(PM, createFCFGSimplificationPass()); // lav cfg simplification
                                                 //TODO dodati ponovo
                                                 //Optimize(PM);
  }
      //Ovo je prakticno bez optimizacija
      else {
    addPass(PM, createIPConstantPropagationPass()); // IP Constant Propagation
    //  addPass(PM, createDeadArgEliminationPass());   // Dead argument
    // elimination
    //  addPass(PM, createInstructionCombiningPass()); // Clean up after IPCP &
    // DAE
    //  addPass(PM, createCFGSimplificationPass());    // Clean up after IPCP &
    // DAE

    addPass(PM, createLoopSimplifyPass()); //Canonicalize natural loops
                                           //!@#$
    addPass(PM, createLowerSwitchPass());  //Switch ---> branch
                                    //    addPass(PM, createLoopRotatePass());
    addPass(PM, createLCSSAPass()); //loop closed ssa form
    addPass(PM, createFCFGSimplificationPass()); // lav cfg simplification
    addPass(PM, createFLoopUnrollPass());        // lav unroll
                                                 //#!
    //addPass(PM, createFCFGSimplificationPass());    // lav cfg simplification

    //  addPass(PM, createPromoteMemoryToRegisterPass());// Kill useless allocas
    //addPass(PM, createFCFGSimplificationPass());    // lav cfg simplification
  }
}

int solvererror() {
  std::cerr << "\n\n-----------------------------------------------------------"
               "---------" << std::endl;
  std::cerr << "You must add a solver (Boolector, Z3, MATHSat or Yices) and "
               "recompile LAV.\nRead README file for instructions."
            << std::endl;
  std::cerr << "---------------------------------------------------------------"
               "-----\n\n" << std::endl;
  exit(EXIT_FAILURE);
}

bool solverDefined() {
#if defined(BOOLECTOR)
  //  std::cout << "boolector defined \n " << std::endl;
  return true;
#endif
#if defined(YICES)
  //  std::cout << "yices defined \n " << std::endl;
  return true;
#endif
#if defined(Z3)
  //  std::cout << "z3 defined \n " << std::endl;
  return true;
#endif
#if defined(MATHSAT)
  //  std::cout << "mathsat defined \n " << std::endl;
  return true;
#endif

  return false;
}

void CheckSolver() {
  if (!solverDefined())
    solvererror();
}
