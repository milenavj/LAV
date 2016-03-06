//===-- FLoop.h -----------------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef FLOOP_H
#define FLOOP_H

#include "llvm/Analysis/Passes.h"

namespace lav {
llvm::Pass *createFLoopUnrollPass();
}

#endif   
