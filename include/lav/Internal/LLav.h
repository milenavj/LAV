//===-- FLav.h ------------------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef FLAV_H
#define FLAV_H

#include "llvm/Analysis/Passes.h"

namespace lav { llvm::Pass *createLavPass(); }

#endif
