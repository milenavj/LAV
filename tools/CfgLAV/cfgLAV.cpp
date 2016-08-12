//===-- main.cpp ----------------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "config.h"
#include "lav/Transformation/LSimplify.h"
#include "lav/Transformation/InlineAll.h"
#include "lav/Misc/misc.h"

#include "llvm/IR/Module.h"
#include "llvm/Support/CallSite.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/system_error.h"

#include "llvm/PassManager.h"
#include "llvm/Support/PassNameParser.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

using namespace llvm;
using namespace lav;

namespace {
llvm::cl::opt<std::string> OutputFolder(
    "output-folder",
    llvm::cl::desc("CfgLAV --- Name of output folder (default = Output)"),
    llvm::cl::init("Output"));

llvm::cl::opt<bool> EnableOptimizations(
    "enable-optimizations",
    llvm::cl::desc("CfgLAV --- Enable LLVM optimizations (default = false)"),
    llvm::cl::init(false));

llvm::cl::opt<bool>
    EnableInline("enable-inline",
                 llvm::cl::desc("CfgLAV --- Enable inlining (default = false)"),
                 llvm::cl::init(false));

llvm::cl::opt<int> Threshold(
    "threshold",
    llvm::cl::desc(
        "CfgLAV --- If enable inlining is specified, specify threshold for "
        "inlining (default = 200 --- inline small functions only)"),
    llvm::cl::init(200));

llvm::cl::opt<std::string> InputFileName(cl::desc("<input bytecode>"),
                                         cl::Positional, cl::init("-"));
}

static void DumpCFG(Module *mainModule);

int main(int argc, char **argv) {
  atexit(llvm_shutdown); // Call llvm_shutdown() on exit.
  lav::parseArguments(argc, argv);
  sys::PrintStackTraceOnErrorSignal();

  // Load the bytecode...
  std::cout << std::endl << "Loading the bytecode..." << std::endl;
  std::string ErrorMsg;

  Module *mainModule = 0;

  OwningPtr<MemoryBuffer> BufferPtr;
  llvm::error_code ec =
      MemoryBuffer::getFileOrSTDIN(InputFileName.c_str(), BufferPtr);
  if (ec) {
    lav::exit_error((std::string) "error loading program '%s': %s" +
                    InputFileName.c_str() + ec.message().c_str());
  }
  mainModule =
      getLazyBitcodeModule(BufferPtr.get(), getGlobalContext(), &ErrorMsg);

  if (mainModule) {
    if (mainModule->MaterializeAllPermanently(&ErrorMsg)) {
      delete mainModule;
      mainModule = 0;
    }
  }
  if (!mainModule)
    lav::exit_error((std::string) "error loading program '%s': %s" +
                    InputFileName.c_str() + ErrorMsg.c_str());

  std::cout << "Loading the bytecode... Completed " << std::endl << std::endl;

  PassManager Passes;
  Passes.add(new llvm::DataLayout(mainModule));
  Passes.add(createFCFGSimplificationPass()); // Clean up after IPCP & DAE
  if (EnableInline)
    Passes.add(createAllInlinerPass(
        Threshold)); //Inline malo vece funkcije, parametar inlininga od 200 do
                     //milijardu, ako je bez argumenta postavljeno je na
                     //milijardu

  Passes.run(*mainModule);
  MakeDirectory(OutputFolder);
  DumpCFG(mainModule);

  BufferPtr.take();
  std::cout << "Finished " << std::endl << std::endl;

  return 0;
}

///////////////////////////////////////////////////////////////////////////

static std::string BBToString(llvm::BasicBlock *bb) {
  std::ostringstream str_stream;
  str_stream << bb;
  return str_stream.str();
}

static std::string IIToString(llvm::Instruction *ii) {
  std::ostringstream str_stream;
  str_stream << ii;
  return str_stream.str();
}

static llvm::Function *GetFunction(llvm::Instruction *instr,
                                   unsigned &numArgs) {
  llvm::CallSite cs;
  if (instr->getOpcode() == llvm::Instruction::Call) {
    cs = llvm::CallSite(llvm::cast<llvm::CallInst>(instr));
  } else {
    cs = llvm::CallSite(llvm::cast<llvm::InvokeInst>(instr));
  }
  numArgs = cs.arg_size();
  return cs.getCalledFunction();
}

static void DumpCFG(Module *mainModule, std::ofstream &of,
                    std::ofstream &ofNodes) {

  unsigned num_blocks = 0;
  std::map<std::string, unsigned> blokovi;

  //ime funkcije, adresa ulaznog i adresa izlaznog cvora
  std::map<std::string, std::pair<std::string, std::string> > funkcija;

  llvm::Module::iterator fnIt = mainModule->begin(), fn_ie = mainModule->end();

  for (; fnIt != fn_ie; ++fnIt) {
    if (fnIt->isDeclaration()) // && !fnIt->use_empty())
      continue;

    llvm::Function *f = fnIt;
    llvm::Function::iterator bbIt = f->begin(), bbIe = f->end(),
                             bbIee = f->end();

    //FIXME
    //pretpostavimo da je prvi ulazni a da je poslednji izlazni, mada to ne mora
    //da bude slucaj
    llvm::BasicBlock *bbu = bbIt;
    llvm::BasicBlock *bbi = (--bbIee);
    std::string addr_ulaznog = BBToString(bbu);
    std::string addr_izlaznog = BBToString(bbi);
    //postavili smo koji je za ovu funkciju ulazni a koji izlazni
    funkcija.insert(
        std::pair<std::string, std::pair<std::string, std::string> >(
            f->getName(),
            std::pair<std::string, std::string>(addr_ulaznog, addr_izlaznog)));

    for (; bbIt != bbIe; ++bbIt) {
      llvm::BasicBlock *bb = bbIt;
      //Proveriti sta raditi sa blokovima koji nemaju ime
      if (bb->hasName()) {
        num_blocks++;

        ofNodes << std::endl << num_blocks << std::endl;

        std::string addr = BBToString(bb);
        blokovi.insert(std::pair<std::string, unsigned>(addr, num_blocks));

        llvm::BasicBlock::iterator ii = bb->begin(), ie = bb->end();
        for (; ii != ie; ++ii) {
          llvm::Instruction *i = ii;
          if (i->getOpcode() == llvm::Instruction::Invoke ||
              i->getOpcode() == llvm::Instruction::Call) {
            unsigned numArgs;
            llvm::Function *f = GetFunction(i, numArgs);

            if (!f) {
              CallSite cs;
              if (i->getOpcode() == Instruction::Call) {
                cs = CallSite(cast<CallInst>(i));
              } else {
                cs = CallSite(cast<InvokeInst>(i));
              }
              // special case the call with a bitcast case
              llvm::Value *fp = cs.getCalledValue();
              llvm::ConstantExpr *ce = dyn_cast<llvm::ConstantExpr>(fp);

              if (ce && ce->getOpcode() == llvm::Instruction::BitCast) {
                f = dyn_cast<llvm::Function>(ce->getOperand(0));
                num_blocks++;

                std::string instruct;
                llvm::raw_string_ostream rso(instruct);
                i->print(rso);
                ofNodes << rso.str();
                ofNodes << std::endl;
                //              ofNodes << *i  <<std::endl;
                ofNodes << std::endl << num_blocks << std::endl;
                std::string addr = IIToString(i);
                blokovi.insert(
                    std::pair<std::string, unsigned>(addr, num_blocks));
              }
            } //end of if(!f)
                else if (f && f->isDeclaration()) {
              std::string instruct;
              llvm::raw_string_ostream rso(instruct);
              i->print(rso);
              ofNodes << rso.str();
              ofNodes << std::endl;
              //            ofNodes << *i <<std::endl;
              continue;
            }
                //ovo je ako imamo negde definiciju funckije
                else {
              std::string instruct;
              llvm::raw_string_ostream rso(instruct);
              i->print(rso);
              ofNodes << rso.str();
              ofNodes << std::endl;
              //            ofNodes << *i <<std::endl;
              num_blocks++;
              ofNodes << std::endl << num_blocks << std::endl;

              std::string addr = IIToString(i);
              blokovi.insert(
                  std::pair<std::string, unsigned>(addr, num_blocks));
            }
          } //end od if-a

          std::string instruct;
          llvm::raw_string_ostream rso(instruct);
          i->print(rso);
          ofNodes << rso.str();
          ofNodes << std::endl;
          //        ofNodes << *i <<std::endl;
        } //end od for-a
      }   // end od if(bb->hasName())
    }     //end od prolaska kroz kroz funkciju
  } // kraj prolaska kroz sve funkcije, trebalo bi da imamo sve brojeve i svu
    // tabelu

  of << num_blocks << std::endl;

  ofNodes.close();

  fnIt = mainModule->begin(), fn_ie = mainModule->end();
  for (; fnIt != fn_ie; ++fnIt) {

    if (fnIt->isDeclaration()) // && !fnIt->use_empty())
      continue;

    llvm::Function *f = fnIt;

    llvm::Function::iterator bbIt = f->begin(), bbIe = f->end();
    //Dodavanje grana blokova koji imaju ime
    for (bbIt = f->begin(), bbIe = f->end(); bbIt != bbIe; ++bbIt) {
      llvm::BasicBlock *bb = bbIt;
      //FIXME Proveriti sta raditi sa blokovima koji nemaju ime???
      if (bb->hasName()) {
        unsigned current_block = blokovi.find(BBToString(bb))->second;

        llvm::BasicBlock::iterator ii = bb->begin(), ie = bb->end();
        for (; ii != ie; ++ii) {
          llvm::Instruction *i = ii;
          if (i->getOpcode() == llvm::Instruction::Invoke ||
              i->getOpcode() == llvm::Instruction::Call) {
            unsigned numArgs;
            llvm::Function *ff = GetFunction(i, numArgs);

            if ((ff && ff->isDeclaration()))
              continue;

            if (!ff) {
              CallSite cs;
              if (i->getOpcode() == Instruction::Call) {
                cs = CallSite(cast<CallInst>(i));
              } else {
                cs = CallSite(cast<InvokeInst>(i));
              }
              // special case the call with a bitcast case
              llvm::Value *fp = cs.getCalledValue();
              llvm::ConstantExpr *ce = dyn_cast<llvm::ConstantExpr>(fp);

              if (ce && ce->getOpcode() == llvm::Instruction::BitCast) {
                ff = dyn_cast<llvm::Function>(ce->getOperand(0));
                std::string addr = IIToString(i);
                unsigned blok_koji_odgovara_instrukciji =
                    blokovi.find(addr)->second;
                unsigned ulazni_cvor_u_funkciju = blokovi
                    .find(funkcija.find(ff->getName())->second.first)->second;
                unsigned izlazni_cvor_iz_funkcije = blokovi
                    .find(funkcija.find(ff->getName())->second.second)->second;
                //current_block spojiti granom sa ulaznim cvorom
                of << current_block << " " << ulazni_cvor_u_funkciju
                   << std::endl;
                //izlazni cvor spojiti sa blokom koji odgovara ovoj instrukcji
                of << izlazni_cvor_iz_funkcije << " "
                   << blok_koji_odgovara_instrukciji << std::endl;
                //naci cvor koji odgovara ovoj instrukciji i to je onda novi
                //current_block
                current_block = blok_koji_odgovara_instrukciji;
              }
            }
                //ovo je ako imamo negde definiciju funckije
                else {
              std::string addr = IIToString(i);
              unsigned blok_koji_odgovara_instrukciji =
                  blokovi.find(addr)->second;
              unsigned ulazni_cvor_u_funkciju = blokovi
                  .find(funkcija.find(ff->getName())->second.first)->second;
              unsigned izlazni_cvor_iz_funkcije = blokovi
                  .find(funkcija.find(ff->getName())->second.second)->second;

              //current_block spojiti granom sa ulaznim cvorom
              of << current_block << " " << ulazni_cvor_u_funkciju << std::endl;
              //izlazni cvor spojiti sa blokom koji odgovara ovoj instrukcji
              of << izlazni_cvor_iz_funkcije << " "
                 << blok_koji_odgovara_instrukciji << std::endl;

              //naci cvor koji odgovara ovoj instrukciji i to je onda novi
              //current_block
              current_block = blok_koji_odgovara_instrukciji;
            }
          } //end od if-a
        }   //end od for-a

        llvm::succ_iterator it = llvm::succ_begin(bb), ite = llvm::succ_end(bb);
        for (; it != ite; it++) {
          llvm::BasicBlock *succ = *it;
          of << current_block << " " << blokovi.find(BBToString(succ))->second
             << std::endl;
        }
      }
    }
  }
}

/*
static void KorpusZaKlasifikaciju(std::ofstream& of, std::ofstream& ofNodes)
{

    std::string s1 = InputFileName;
    s1[s1.size()-2]='_';
    s1[s1.size()-1]='_';

    int i;
    for(i=s1.size()-1; i>=0; i--)
        if(s1[i] == '/') break;

    i++;
    std::string s;
    s.resize(s1.size()-i);
    for(unsigned k=0; k<s1.size()-i; k++)
    s[k] = s1[i+k];

    MakeDirectory(OutputFolder);
    std::string nodes = OutputFolder + "/Nodes/";
    std::string cfg = OutputFolder + "/CFG/";
    std::string cfile = OutputFolder + "/C/";


    MakeDirectory(nodes.c_str());
    MakeDirectory(cfg.c_str());
    MakeDirectory(cfile.c_str());

    std::string sufixFileName = OutputFolder + "/Sufix";
    std::ifstream sufixFileI(sufixFileName.c_str());
    int suf = 0;
    sufixFileI >> suf;
    std::string sufix = ItoS(suf);
    sufixFileI.close();
    suf++;
    std::ofstream sufixFileO(sufixFileName.c_str());
    sufixFileO << suf << std::endl;
    sufixFileO.close();

    std::string nodeName  = nodes + sufix + "_" + s;
    ofNodes.open(nodeName.c_str());

    std::string cfgName = cfg + sufix + "_" + s;
    of.open(cfgName.c_str());

    std::string filenames = cfile;
    std::ofstream fn(filenames.c_str(), std::ios::app);
    fn << sufix + "_" + s<< std::endl;
    fn.close();


    std::string file_cfg = cfile+ sufix + "_" + s + ".c";
    std::string cfilename = InputFileName;
    cfilename[cfilename.size()-2]='.';
    cfilename[cfilename.size()-1]='c';
    CopyFile(cfilename.c_str(), file_cfg.c_str());
}

*/

static int OpenCFGAndNodesFiles(std::ofstream &of, std::ofstream &ofNodes) {
  std::string s_cfg = InputFileName;
  s_cfg[s_cfg.size() - 1] = 'c';
  s_cfg.push_back('f');
  s_cfg.push_back('g');

  std::string s_nodes = InputFileName;
  s_nodes[s_nodes.size() - 1] = 'n';
  s_nodes.push_back('o');
  s_nodes.push_back('d');

  ofNodes.open(s_nodes.c_str());
  of.open(s_cfg.c_str());
  if (!ofNodes)
    exit_error("OpenCFGAndNodesFiles::Could not open " + s_cfg + " file!");
  if (!of)
    exit_error("OpenCFGAndNodesFiles::Could not open " + s_nodes + " file!");

  std::string filenames = OutputFolder + "/files";
  std::ofstream fn(filenames.c_str(), std::ios::app);
  if (!fn)
    exit_error("OpenCFGAndNodesFiles::Could not open " + filenames + " file!");

  fn << s_cfg << std::endl;
  fn << s_nodes << std::endl;
  fn.close();

  return 1;
}

static void DumpCFG(Module *mainModule) {
  std::ofstream nodes;
  std::ofstream cfg;

  //    KorpusZaKlasifikaciju(cfg, nodes);
  OpenCFGAndNodesFiles(cfg, nodes);
  DumpCFG(mainModule, cfg, nodes);

  cfg.close();
  nodes.close();
}
