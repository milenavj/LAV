//===-- FSimplify.h -------------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef FSIMPLIFY_H
#define FSIMPLIFY_H

#include "llvm/Analysis/Passes.h"

namespace lav {
llvm::Pass *createFCFGSimplificationPass();
}  

#endif
