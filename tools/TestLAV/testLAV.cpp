//===-- test.cpp ----------------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//


#include "lav/Misc/misc.h"
#include "lav/Test/test.h"

#include <signal.h>

using namespace lav; 

int main(int argc, char** argv)
{
  signal(SIGTERM, &termination_handler);
  lav::parseArguments(argc, argv);
  Test(Directory().c_str(), TestFile().c_str(), LavOptions().c_str(), TestMode(), SameOptions(), ModelFile().c_str(), Model());
  return 0; 
}

