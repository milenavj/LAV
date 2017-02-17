//===-- InlineAll.h
//-----------------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef FINLINE_H
#define FINLINE_H

#include "llvm/Analysis/Passes.h"

namespace lav {
llvm::Pass *createAllInlinerPass();
llvm::Pass *createAllInlinerPass(int Threshold);
}

#endif
