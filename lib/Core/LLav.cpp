//===-- LLav.cpp ----------------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <stdlib.h>
#include <iostream>
#include <fstream>

#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/Timer.h"
#include "llvm/Support/CommandLine.h"

#include "lav/Internal/LModule.h"

using namespace llvm;
using namespace lav;

namespace {



class LavPass : public ModulePass {
public:
    static char ID; // Pass identification, replacement for typeid

    LavPass() : ModulePass(ID) {}
    
    virtual bool runOnModule(Module &M);

    virtual void getAnalysisUsage(AnalysisUsage &AU) const {
      AU.setPreservesAll();
    }
private:

  };
}

char LavPass::ID = 0;
static RegisterPass<LavPass> X("LavPass", "LavPass analysis");

namespace lav{
// Public interface to the Lav pass
ModulePass *createLavPass() {
  return new LavPass();
}
}
 
bool LavPass::runOnModule(Module &M)
{

  Timer PrintingTime("Printing results");
  std::cout << "Generating and testing conditions... " << std::endl;
  LModule LavModule(&M);
  LavModule.Run();
  std::cout << "Generating and testing conditions... Completed " 
            << std::endl << std::endl;


  std::cout << "Printing results ... " << std::endl;
  PrintingTime.startTimer();
  LavModule.PrintResults();
  PrintingTime.stopTimer();
  std::cout << "Printing results ... Completed " << std::endl << std::endl;

  //modul nije modifikovan ovim pasom
  return false;
}
