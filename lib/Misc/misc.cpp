//===-- misc.cpp ----------------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <stdio.h>
#include <string.h>

#include "llvm/Support/CommandLine.h"
#include "lav/Misc/misc.h"




namespace lav
{


static std::string strip(std::string &in) {
  unsigned len = in.size();
  unsigned lead = 0, trail = len;
  while (lead<len && isspace(in[lead]))
    ++lead;
  while (trail>lead && isspace(in[trail-1]))
    --trail;
  return in.substr(lead, trail-lead);
}

static void readArgumentsFromFile(char *file, std::vector<std::string> &results) {
  std::ifstream f(file);
  assert(f.is_open() && "unable to open input for reading arguments");
  while (!f.eof()) {
    std::string line;
    std::getline(f, line);
    line = strip(line);
    if (!line.empty())
      results.push_back(line);
  }
  f.close();
}

void parseArguments(int argc, char **argv) {
  std::vector<std::string> arguments;

  for (int i=1; i<argc; i++) {
    if (!strcmp(argv[i],"--read-args") && i+1<argc) {
      readArgumentsFromFile(argv[++i], arguments);
    } else {
      arguments.push_back(argv[i]);
    }
  }
    
  int numArgs = arguments.size() + 1;
  const char **argArray = new const char*[numArgs+1];
  argArray[0] = argv[0];
  argArray[numArgs] = 0;
  for (int i=1; i<numArgs; i++) {
    argArray[i] = arguments[i-1].c_str();
  }

  llvm::cl::ParseCommandLineOptions(numArgs, (const char**) argArray, " LAV\n");
  delete[] argArray;
}


std::string sErrorKind(ERRKIND ErrKind)        
{
if(ErrKind == BUFFEROVERFLOW) return "buffer_overflow";
else if(ErrKind == DIVISIONBYZERO) return "division_by_zero";
else if(ErrKind == ASSERT) return "assertion_violated";
else if(ErrKind == NULLDEREFERENCING) return "null_pointer_dereferencing";
else return "other";
}


void perror(const std::string& s)
{
//std::cout << "Error: " << s << std::endl;
std::cerr << "Error: " << s << std::endl;
}

void exit_error(const std::string& s)
{
perror(s);
exit(EXIT_FAILURE);
}

void perror(const std::string& s, const std::string& fileName)
{
std::ofstream f(fileName.c_str(), std::ios::app);
if(!f) perror("could not open log file: "+fileName);
f << "Error: " << s << std::endl;
perror(s);
f.close();
}

void exit_error(const std::string& s, const std::string& fileName)
{
perror(s, fileName);
exit(EXIT_FAILURE);
}


int MakeDirectory(std::string s)
{
int sys;
std::string command("mkdir ");
command += s;
command += (std::string)(" 2>/dev/null"); 
sys = system(command.c_str());
return sys;
}

std::string ExtractFileName(const std::string& in)
{
  std::string result;
  int j;
  for( j = in.size() - 1; j>=0; j--)
      if(in[j] == '/') break;

  for(unsigned i = j+1; i< in.size(); i++)
    if(in[i] != '.') result+=in[i];
    else break;

  return result;
}

std::string ItoS(long long num)
{
  std::ostringstream str_stream;
  str_stream << num;
  return str_stream.str();
}

int CopyFile(const char* source, const char* dest)
{
    FILE* s = fopen(source,"r");
    FILE* d = fopen(dest,"w");
    int c;
    if(s==NULL) { perror((std::string)"Error while coping:\n Could not open source file: " + std::string(source)); return 1;}
    if(d==NULL) { perror((std::string)"Error while coping:\n Could not open destination file: " + std::string(dest)); return 1;}
    while((c=fgetc(s))!=EOF) 
        fputc(c,d);
    fclose(s);
    fclose(d);
    return 0;
}


int GetLine(const std::string &fileName, unsigned lno, std::string& line)
{
    std::ifstream input(fileName.c_str());
    if(!input.is_open()) return 0;
    unsigned i = 0;
    while(!input.eof() && i<lno)
    {
        std::getline(input, line); 
        i++;
    }
    if(i<lno) return 0;
    input.close();
    return 1;
}

int Delete(std::string name)
{
    int sys;
    std::string del("rm ");
    del += name;    
    sys = system(del.c_str());
    return sys;
}

bool StartsWith(const std::string& s, const std::string& begin)
{
    if((s.size() < begin.size())) return false;
    for(unsigned i=0; i<begin.size(); i++)
      if(s[i] != begin[i]) return false;
    return true;
}

bool EndsWith(const std::string& s, const std::string& end)
{

  if((s.size() < end.size())) return false;
  int j = (s.length() - 1);
  int i = (end.length() - 1);
  while (s[j] == '\0') j--;
  while (end[i] == '\0') i--;

  for( ; i>=0; i--, j--)
    if(s[j] != end[i]) {return false;}
  return true;
}

}//end of namespace
