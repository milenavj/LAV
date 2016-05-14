//===-- LModule.cpp -------------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <iostream>
#include <fstream>

#include "llvm/IR/Module.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"

#include "lav/Internal/LCommon.h"
#include "lav/Internal/LExpressionTransformation.h"
#include "lav/Internal/LModule.h"
#include "lav/Internal/LFunction.h"
#include "lav/Internal/LConstraints.h"
#include "lav/Internal/InstructionInfoTable.h"
#include "lav/Misc/misc.h"

#include "expression/output/SMTFormater.h"

extern llvm::cl::opt<std::string> StartFunction;
extern llvm::cl::opt<std::string> InputFile;
extern llvm::cl::opt<std::string> OutputFolder;
extern llvm::cl::opt<bool> PrintHtml;

namespace lav
{
LModule::LModule(llvm::Module* m) :_Module(m),  
                                    _TargetData(new llvm::DataLayout(m)),
                                    _Infos(new InstructionInfoTable(m)),
                                    _GlobalMemoryOffset(1)
{ init(); }

/*
void LModule::CalculateConditions()
{
    int b = 0;

    for(unsigned i=0; i<_Functions.size(); i++) 
      if(_Functions[i]->GetFunctionName() == StartFunction) 
        {_Functions[i]->CalculateConditions(); b++; break;}

    //Ovo je ako modul nema main da bi se izracunalo za sve funkcije
    if(b==0)
      for(unsigned i=0; i<_Functions.size(); i++) 
        _Functions[i]->CalculateConditions();
}

void LModule::CalculateDescriptions()
{
  int b = 0;
    for(unsigned i=0; i<_Functions.size(); i++) 
      if(_Functions[i]->GetFunctionName() == StartFunction) 
       {_Functions[i]->CalculateDescriptions(); b++;}
  if(b==0)
    for(unsigned i=0; i<_Functions.size(); i++) 
	    _Functions[i]->CalculateDescriptions();
}*/

void LModule::Run()
{
  int b = 0;

  for(unsigned i=0; i<_Functions.size(); i++) 
    if(_Functions[i]->GetFunctionName() == StartFunction)      
        {_Functions[i]->Run(); b++; break;    }

  if(b==0)
    for(unsigned i=0; i<_Functions.size(); i++) 
    { _Functions[i]->Run(); }
}

LModule::~LModule()
{ 
  for(unsigned i=0; i<_Functions.size(); i++)
      delete _Functions[i];
  delete _Infos;
  delete _TargetData;
}


std::ostream& LModule::Print(std::ostream& ostr) const
{
  for(unsigned i=0; i<_Functions.size(); i++)
          _Functions[i]->Print(ostr);
  return ostr;	
}

std::ostream& LModule::PrintHTMLHeader(std::ostream& f) const
{
  f << "<html>" << std::endl 
    << "<head> " << std::endl 
    << "<title> Results </title>" << std::endl 
    << "<head>" << std::endl 
    << "<body bgcolor = \"#F0E0BF\">" << std::endl;

  f << "<h1> <font face = \"Courier New\">Results </font></h1>" << std::endl;
  f << "<h3> <font face = \"Courier New\"> " << InputFile<<  " </font></h3>" << std::endl;

  return f;
}

std::ostream& LModule::PrintHTMLFooter(std::ostream& f) const
{
  f << "<hr>" << endl;
  f << "<p><font face = \"Courier New\" size = \"1\"> Not for commercial use. This report is provided \"as is\" and without any expressed or implied warranties. <font></p>" << endl;
//  f << "<br><br>" << endl;
//  f << "<hr>" << std::endl;
  f << "<p><font face = \"Courier New\" size = \"1\">All commands are classified into following categories:" << std::endl;
  f << "<ul>" << std::endl;
  f << "<li> <font face = \"Courier New\" size = \"1\" color = \"#008080\"> SAFE </font> <font face = \"Courier New\" size = \"1\" > - this command never (independently of input data or control flow) leads to an error.  </font></li>" << std::endl;
  f << "<li> <font face = \"Courier New\" size = \"1\" color = \"red\"> FLAWED </font> <font face = \"Courier New\" size = \"1\" > - this command always (independently of input data or control flow) leads to an error. </font></li>" << std::endl;
  f << "<li> <font face = \"Courier New\" size = \"1\" color = \"brown\"> UNSAFE </font> <font face = \"Courier New\" size = \"1\" > - this command can (for some input data or control flow) lead to an error. </font></li>" << endl;
  f << "<li> <font face = \"Courier New\" size = \"1\" color = \"orange\"> UNREACHABLE </font> <font face = \"Courier New\" size = \"1\" > - this command is never reached during the code execution. </font></li>" << std::endl;
  f << "</ul>" << std::endl;
//  f << "<hr>" << std::endl;
//  f << "<p><font face = \"Courier New\"><a href = \"transformisan_kod\" target=\"_blank\"> Transformed code </a></font><br>" <<std::endl;
  f << "<hr>" << endl;
  f << "</body>" << endl << "</html>" << endl;
  return f;
}

void LModule::Print(const std::string& sFilename) const
{
  std::ofstream f(sFilename.c_str());
  if(!f.is_open()) {perror(((std::string)"LModule::Print Could not open txt file "+ sFilename)); return;}
  Print(f);
  f.close();
}

void LModule::PrintHTMLHeader(const std::string& sFilename) const
{
  std::ofstream f(sFilename.c_str());
  if(!f.is_open()) {perror(((std::string)"LModule::PrintHTMLHeader Could not open html file "+ sFilename)); return;}
  PrintHTMLHeader(f);
  f.close();
}

void LModule::PrintHTMLFooter(const std::string& sFilename) const
{
  std::ofstream f(sFilename.c_str(), std::ios::app);
  if(!f.is_open()) {perror(((std::string)"LModule::PrintHTMLFooter Could not open html file "+ sFilename)); return;}
  PrintHTMLFooter(f);
  f.close();
}

void LModule::PrintHTML(const std::string& sFilename) const
{
  std::ofstream f(sFilename.c_str());
  if(!f.is_open()) {perror(((std::string)"LModule::PrintHTML Could not open html file "+ sFilename)); return;}

  PrintHTMLHeader(f);
  f << "<h2> <font face = \"Courier New\">List of commands checked</font></h2>" << endl;

  f << "<ol>" << endl;
  for(unsigned i=0; i<_Functions.size(); i++)
            _Functions[i]->PrintHTML(f);
  f << "</ol>" << endl;
  PrintHTMLFooter(f);
  f.close();
}


LFunction* LModule::GetLFunction(llvm::Function* f) const
{ 
  std::map<llvm::Function*, LFunction*>::const_iterator iter = _LlvmToF.find(f);
  if(iter == _LlvmToF.end()) return NULL;
  return iter->second;    
}

argo::Expression LModule::GetGlobalConstraints() const { 
//return MakeANDFromExpressions(_GlobalConstraints); 
return _GlobalConstraints.Constraint(); 
}

void LModule::AddGlobalVariables()
{
 for (llvm::Module::global_iterator I = _Module->global_begin(), E = _Module->global_end();
 I != E; ++I) {
  if(llvm::GlobalVariable* gv = llvm::dyn_cast<llvm::GlobalVariable>(I))
  { 
    if(isRelevantGlobalVariable(gv))
    {
        _GlobalVariables.insert(gv);
    }
  }
 }

}
static argo::SMTFormater SMTF;


void LModule::GetAddresses(std::vector<argo::Expression>& eqs) const
{
  //FIXME globalni malloc nije uzet u obzir!!!!
  std::map<std::string, unsigned>::const_iterator it = _Addresses.begin(), ie = _Addresses.end();
  for( ; it!=ie; ++it)  
  {
  //FIXME zbog free dodati proveru da li je second nula
    argo::Expression e ;
    if(isAddress(it->first)) e = argo::Expression::Equality(ExpVar(it->first, fpointer_type, false), ExpNum1(it->second, fpointer_type));
/*    else if(isMalloc(it->first))
    {
    argo::Expression e1 = argo::Expression::Equality(ExpGlobalAddress(it->first), ExpNum1(current, fpointer_type));
    argo::Expression e2 = argo::Expression::Equality(ExpGlobalAddress(it->first), ExpNumZeroPtr);
    e = argo::Expression::OR(e1, e2);
    }
    else e = argo::Expression::Equality(ExpGlobalAddress(it->first), ExpNum1(current, fpointer_type));
    current += it->second;*/
    eqs.push_back(e);
  }
}

void LModule::PrintResults() const
{
  std::string oname= OutputFolder + "/" + ExtractFileName(InputFile);
  Print(oname + ".txt");  
  if(PrintHtml)
    PrintHTML(oname + ".html");
}

void LModule::init()
{

  AddGlobalVariables();
  unsigned i =0;

  llvm::Module::iterator fnIt = _Module->begin(), fn_ie = _Module->end();
  fnIt = _Module->begin();
  for (; fnIt != fn_ie; ++fnIt) {
    if (fnIt->isDeclaration())// && !fnIt->use_empty())
        continue;
     llvm::Function* f = fnIt;
    _Functions.push_back(new LFunction(f, this));
    _LlvmToF[f] = _Functions[i++]; 
    }

 llvm::Module::global_iterator I = _Module->global_begin(), 
                               E = _Module->global_end();
 for ( ; I != E; ++I) {
  if(llvm::GlobalVariable* gv = llvm::dyn_cast<llvm::GlobalVariable>(I))
  { 
    if(isRelevantGlobalVariable(gv))
    {
        const llvm::SequentialType* stype = llvm::dyn_cast<llvm::SequentialType>(gv->getType());
        llvm::Type* t = stype->getElementType();
        argo::Expression e = ExpGlobalAddress(GetOperandName(gv));
      if(llvm::dyn_cast<llvm::PointerType>(t))
      { 
        //Ako je alociran pointer njegovi left i right se postavljaju na nulu
        _GlobalConstraints.AddConstraint(e, 0, 0);
        //FIXME ako je pointer inicijalizovan
        //FIXME nedostaje pomeraj*1000
        _Addresses.insert(std::pair<std::string, unsigned>(e.GetName(), _GlobalMemoryOffset));
        _GlobalConstraints.AddAddressConstraint(_GlobalMemoryOffset, e,fpointer_type/8);
      }
      else
      {
        //ako je npr niz ili struktura postave se granice u bitovima 
        if(t->isSized() && t->isAggregateType())
        {
          // ovo vraca sve poravnato na prvu vecu vrednost deljivu sa 4
          //nije jasno koja je razlika izmedju naredna dve funkcije?
          //unsigned size = GetParentModule()->GetTargetData()->getTypeStoreSize(t);
          unsigned size = GetTargetData()->getTypeAllocSize(t);
          _GlobalConstraints.AddConstraint(e, 0, size);
          _Addresses.insert(std::pair<std::string, unsigned>(e.GetName(), _GlobalMemoryOffset));
          _GlobalConstraints.AddAddressConstraint(_GlobalMemoryOffset, e, size);
        }
        else
        {//FIXME za razlicite tipove
        }
      }
    }//end if(isRelevantGlobalVariable(gv))
  }

} //kraj petlje
}


} //end of namespace
