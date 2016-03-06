//===-- test.h ----------------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef TEST_H
#define TEST_H

#include <string>  

namespace lav {
bool TestMode(); 
bool SameOptions(); 
bool Model();
std::string ModelFile();
std::string LogFile();
std::string Directory();
std::string TestFile();
std::string LavOptions();

void Test(const char* directoryName, const char* testFileName, const char* lavOptions, bool check, bool sameOptions, const char* modelFile, bool model);

void termination_handler(int signum);
}
#endif

