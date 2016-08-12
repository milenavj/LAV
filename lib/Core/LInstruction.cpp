//===-- LInstruction.cpp
//---------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <iostream>
#include <fstream>

#include "lav/Internal/LInstruction.h"
#include "lav/Internal/LBlock.h"
#include "lav/Internal/LCommon.h"

#include "lav/Internal/InstructionInfoTable.h"
#include "llvm/IR/Instruction.h"

#include "llvm/Support/raw_ostream.h"

extern llvm::cl::opt<std::string> StartFunction;
extern llvm::cl::opt<std::string> InputFile;
extern llvm::cl::opt<std::string> OutputFolder;
extern llvm::cl::opt<bool> PrintHtml;
extern llvm::cl::opt<bool> Model;

//////////////////////////////////////////////////////////////
// LInstruction
//////////////////////////////////////////////////////////////
namespace lav {

vUnsigned LInstruction::GetStackLines() const {
  vUnsigned stack;
  if (_Stack.size() > 0)
    for (int i = _Stack.size() - 1; i >= 0; i--)
      stack.push_back(_Stack[i]->_Info->line);
  stack.push_back(_Info->line);
  return stack;
}

std::ostream &LInstruction::Print(std::ostream &ostr) const {
  if (_Stack.size() > 0)
    for (int i = _Stack.size() - 1; i >= 0; i--) {
      ostr << " line: " << _Stack[i]->_Info->line << " ";
      //if(GetLine(cfile, _Stack[i]->_Info->line, code))
      //ostr << "\n code: "<< code << "\n ---> " << std::endl ;
    }

  //ostr << *_Instruction;
  //ostr << "id: " << _Info->id
  //<< " file: " << getFileName(_Info->file)
  ostr << " line: " << _Info->line << "   ";
  //ostr << " assembly line: " << _Info->assemblyLine;
  //ostr << std::endl;
  //if(GetLine(cfile, _Info->line, code))
  //   ostr << "\n code: "<< code << std::endl ;
  return ostr;

}

std::ostream &LInstruction::PrintHTML(std::ostream &f) const {
  if (!PrintHtml)
    return f;

  f << std::endl << "<br><font face = \"Courier New\">";

  if (_Stack.size() > 0) {
    for (int i = _Stack.size() - 1; i >= 0; i--) {
      std::string s;
      llvm::raw_string_ostream rso(s);
      rso << *_Stack[i]->Instruction();
      f << rso.str();
      f << "<br>";
      f << "<tt>" << _Stack[i]->_Info->line << std::endl << " ---> </tt>"
        << std::endl;
    }
    f << "\n<br>\n";
  }

  std::string s;
  llvm::raw_string_ostream rso(s);
  rso << *_Instruction;
  f << rso.str();

  f << "<br>" << std::endl;
  f << " file: <tt><a href=\"" << _Info->file << "\" target = \"blank\">"
    << (_Info->file) << "</a></tt>" << std::endl;
  f << "<br>" << std::endl;

  std::string code;
  std::string cfile = InputFile;
  cfile[cfile.length() - 1] = 'c';
  if (_Stack.size() > 0) {
    for (int i = _Stack.size() - 1; i >= 0; i--) {
      unsigned line = _Stack[i]->_Info->line;
      f << "</tt> line: <tt>" << line << std::endl;
      if (GetLine(cfile, line, code))
        f << "</tt> <br> code: <tt>" << code << "<br> ---> <br>" << std::endl;
      else
        f << " ---> " << std::endl;
    }
  }

  f << "</tt> " << std::endl << "line: <tt>" << _Info->line << std::endl;
  if (GetLine(cfile, _Info->line, code))
    f << "</tt> <br> code: <tt>" << code << std::endl;
  /*
    if(Model())
    {
    f  << "</tt><br>"<< std::endl;
    std::string s = ExtractFileName(GetModelFileName());
    f << "<tt><a href=\"" << s <<"\">" << "Model" << "</a>";
    }*/
  f << "</tt><br></font> " << std::endl;
  return f;
}

std::string LInstruction::GetModelFileName() const {
  std::string filename(OutputFolder + "/");
  filename += GetParentBlock()->GetFunctionName();
  filename += (std::string("_line") + "_");

  vUnsigned v = GetStackLines();
  for (unsigned j = 0; j < v.size(); j++)
    filename += (ItoS(v[j]) + "_");
  filename += ("assemblyLine_" + ItoS(Info()->assemblyLine));
  return filename;
}

unsigned LInstruction::AssemblyLine() const { return _Info->assemblyLine; }

void LInstruction::AddIntoStack(const vpInst &stack,
                                const vUnsigned &contexts) {
  assert(stack.size() == contexts.size());
  for (unsigned i = 0; i < stack.size(); i++)
    _Stack.push_back(stack[i]);
  for (unsigned i = 0; i < contexts.size(); i++)
    _Contexts.push_back(contexts[i]);
}

void LInstruction::AddIntoStack(LInstruction *s, unsigned context) {
  _Stack.push_back(s);
  _Contexts.push_back(context);
}

void LInstruction::PrintFlawedFound(std::ostream &ostr) const {
  ostr << std::endl << "VERIFICATION FAILED: " << std::endl;
  Print(ostr);
  ostr << "Assembly line: " << AssemblyLine() << std::endl;
  return;
}

void LInstruction::PrintFlawedFound(const std::string &fileName) const {
  std::ofstream ff(fileName.c_str());
  if (!ff)
    exit_error((std::string) "LInstruction::PrintFlawedFound --- Could not "
                             "open output file: " + fileName);
  PrintFlawedFound(ff);
  ff.close();
}

void LInstruction::PrintFlawedFoundResults(STATUS s) const {
  PrintFlawedFound(std::cerr);
  std::string oname = OutputFolder + "/" + ExtractFileName(InputFile);
  if (PrintHtml)
    PrintFlawedFoundHTML(oname + ".html", s);
  PrintFlawedFound(oname + ".txt");
}

void LInstruction::PrintFlawedFoundHTML(std::ostream &f, STATUS s) const {
  f << std::endl
    << "<br><font face = \"Courier New\" size = \"2\">command: <tt>";
  PrintHTML(f);

  if (s == UNSAFE || s == FLAWED) {
    if (Model) {
      f << "</tt><br>" << std::endl;
      std::string s = ExtractFileName(GetModelFileName());
      f << "<tt><a href=\"" << s << "\">"
        << "Model"
        << "</a>";
    }
    f << "</tt><br></font> " << std::endl;
  }

  f << std::endl << "<font face = \"Courier New\"> status: <tt>";
  if (s == SAFE)
    f << "<font color = \"#008080\">SAFE </font>";
  else if (s == FLAWED)
    f << "<font color = red>FLAWED</font>";
  else if (s == UNSAFE)
    f << "<font color = brown>UNSAFE </font>";
  else if (s == ERROR)
    f << "<font color = red>ERROR </font>";
  else if (s == UNREACHABLE)
    f << "<font color = orange>UNREACHABLE </font>";
  else if (s == UNCHECKED)
    f << "<font color = blue> UNCHECKED</font>";
  else
    f << "???";
  f << "</tt> </font ><br>" << std::endl;
  return;
}

void LInstruction::PrintFlawedFoundHTML(const std::string &fileName,
                                        STATUS s) const {
  std::ofstream f(fileName.c_str(), std::ios::app);
  if (f.is_open()) {
    PrintFlawedFoundHTML(f, s);
    f.close();
  } else
    perror("LInstruction::stopWhenFound --- Could not open html file!");
}

} //end of namespace
