//===-- LTypes.h ---------------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LTYPES_H
#define LTYPES_H
#include <vector>
#include <map>
#include <set>

namespace argo {
class Expression;
}
namespace llvm {     
class Type;
}
namespace lav {

  class LInstruction;
  class LBlock;
  class LLocalCondition;
  class LVariableInfo;
  class LJump;

  typedef argo::Expression                              aExp;
  typedef const argo::Expression                       caExp;
  typedef std::vector<aExp>                             vaExp;
  typedef std::vector<unsigned>                        vUnsigned;
  typedef std::vector<std::string>                      vStr;
  typedef std::vector<LInstruction*>                    vpInst;
  typedef std::vector<LBlock*>                          vpBlock;
  typedef std::vector<LLocalCondition>                  vLocCond;
  typedef std::vector<LVariableInfo>                    vVarInfo;
  typedef std::vector<LJump>                            vJump;

  typedef std::vector< std::pair<unsigned, unsigned> >   vLoop;
  typedef std::map<std::string, LVariableInfo>             msVarInfo;
  typedef std::map<std::string, const aExp*>              mspaExp;
  typedef std::map<std::string, llvm::Type*>               mspType;
  typedef std::set<aExp>                                   saExp;
  typedef const std::string                               cStr;
}


#endif

