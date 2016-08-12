//===-- misc.h ----------------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef MISC_H
#define MISC_H

#include <string>
#include <vector>

namespace lav {
typedef enum status {
  ERROR,
  UNCHECKED,
  SAFE,
  UNSAFE,
  FLAWED,
  UNREACHABLE
} STATUS;
typedef enum errkind {
  OTHER,
  DIVISIONBYZERO,
  BUFFEROVERFLOW,
  NULLDEREFERENCING,
  ASSERT
} ERRKIND;

std::string sErrorKind(ERRKIND er);

void parseArguments(int argc, char **argv);

int MakeDirectory(std::string s);
int CopyFile(const char *source, const char *dest);
int GetLine(const std::string &fileName, unsigned lno, std::string &line);
int Delete(std::string name);

std::string ExtractFileName(const std::string &in);
std::string ItoS(long long num);

void exit_error(const std::string &s);
void exit_error(const std::string &s, const std::string &fileName);
void perror(const std::string &s);
void perror(const std::string &s, const std::string &fileName);

bool EndsWith(const std::string &s, const std::string &end);
bool StartsWith(const std::string &s, const std::string &begin);

}

#endif
