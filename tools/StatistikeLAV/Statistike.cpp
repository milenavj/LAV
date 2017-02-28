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


#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/Dominators.h"
#include "llvm/Analysis/ConstantFolding.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/DominanceFrontier.h"
#include "llvm/Analysis/InstructionSimplify.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/LoopIterator.h"


#include "llvm/PassManager.h"
#include "llvm/Support/PassNameParser.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <math.h>



#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/PassNameParser.h"
#include "llvm/Support/Timer.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/system_error.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/Analysis/Verifier.h"

#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/Scalar.h"


using namespace llvm;
using namespace lav;

namespace {
llvm::cl::opt<std::string> OutputFolder(
    "output-folder",
    llvm::cl::desc("CfgLAV --- Name of output folder (default = Output)"),
    llvm::cl::init("Output"));

llvm::cl::opt<std::string> OutputFileName(
    "output-file-name",
    llvm::cl::desc("Statistike --- Name of output file (default = izlaz)"),
    llvm::cl::init("izlaz"));

llvm::cl::opt<bool> BrojCvorova(
    "cfg-broj-cvorova",
    llvm::cl::desc("Statistike --- CFG broj cvorova"),
    llvm::cl::init(false));

llvm::cl::opt<bool> BrojGrana(
    "cfg-broj-grana",
    llvm::cl::desc("Statistike --- CFG broj grana"),
    llvm::cl::init(false));

llvm::cl::opt<bool> BrojCastova(
    "cfg-broj-kastovanja",
    llvm::cl::desc("Statistike --- CFG Broj kastovanja ukupan"),
    llvm::cl::init(false));

llvm::cl::opt<bool> BrojCastovaFpInt(
    "cfg-broj-kastovanja-fp-int",
    llvm::cl::desc("Statistike --- CFG Broj kastovanja fp-integer"),
    llvm::cl::init(false));

llvm::cl::opt<bool> BrojInstrukcijaPoBloku(
    "cfg-broj-instr-po-bloku",
    llvm::cl::desc("Statistike --- CFG Broj instrukcija po bloku"),
    llvm::cl::init(false));

llvm::cl::opt<bool> BrojInstrukcija(
    "cfg-broj-instr",
    llvm::cl::desc("Statistike --- CFG Broj instrukcija ukupan"),
    llvm::cl::init(false));

llvm::cl::opt<bool> BrojInstrukcijaPoTeluPetlje(
    "cfg-broj-instr-po-telu-petlje",
    llvm::cl::desc("Statistike --- CFG Broj instrukcija po petljama"),
    llvm::cl::init(false));

llvm::cl::opt<bool> IzlazniStepenCvorova(
    "cfg-izlazni-stepen-cvorova",
    llvm::cl::desc("Statistike --- CFG broj grana"),
    llvm::cl::init(false));

llvm::cl::opt<bool> UlazniStepenCvorova(
    "cfg-ulazni-stepen-cvorova",
    llvm::cl::desc("Statistike --- CFG broj grana"),
    llvm::cl::init(false));

llvm::cl::opt<bool> OdnosCvorovaGrana(
    "cfg-odnos-cvorova-grana",
    llvm::cl::desc("Statistike --- CFG odnos broja cvorova i grana"),
    llvm::cl::init(false));

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

static const  std::string separator = ", ";
static int OpenOutputFile(std::ofstream &of, std::string Ext="");

class Podaci {
public:
    Podaci(Module* M) : mainModule(M) {
      MakeDirectory(OutputFolder);
      OpenOutputFile(of);
      if(!of) exit(1);
    }
  void UradiPosao();
private:

    Module* mainModule;
    std::vector<double> statistike; 
    std::vector<double> frekvencije; 
    std::ofstream of;

private:
  int GetBrojCvorova();
  int GetBrojCastova();
  int GetBrojCastovaFpInt();
  int GetBrojGrana();
    void GetUlazniStepenCvorova();
    void GetIzlazniStepenCvorova();
    void GetBrojInstrukcijaPoBloku();
    void GetBrojInstrukcijaPoTeluPetlje();
    int  GetBrojInstrukcija();

    double GetOdnosCvorovaGrana(int c, int g) {
      if(OdnosCvorovaGrana && BrojGrana && BrojCvorova) {
 //       of << "\n" << "OdnosCvorovaGrana" << "\n" ;
        of << separator << (double)c/g;
        return (double)c/g;
    }
      else return 0;
    }

    void Upisi(int n, std::string s = "") {
    if(s!="")
        of << "\n" << s << "\n" ;
        of << separator << n;
    }

    void Ocisti() {
        statistike.clear();
        frekvencije.clear();
        frekvencije.resize(0);
    }

    void OdstampajStatistike() {
//        of << " prosek, min, max, koef, entropy\n" ;
    if(statistike.size() == 0) { of << separator << 0 << separator << 0<< separator << 0<< separator << 0; return ;}

        for(unsigned int i = 0; i<statistike.size(); i++)
            of << separator << statistike[i] ;
    }

    void OdstampajFrekvencije() {

        double fn = 0;
        if(frekvencije.size() == 0) { of << separator << 0 << separator << 0<< separator << 0<< separator << 0; return ;}
        if(frekvencije.size() == 1) { of << separator << frekvencije[0] << separator << 0<< separator << 0<< separator << 0; return ;}
        if(frekvencije.size() == 2) { of << separator << frekvencije[0] << separator << frekvencije[1] << separator << 0<< separator << 0; return ; }
        if(frekvencije.size() == 3) { of << separator << frekvencije[0] << separator << frekvencije[1] << separator << frekvencije[2] << separator << 0;
return ; }
        if(frekvencije.size() == 4) { of << separator << frekvencije[0] << separator << frekvencije[1] << separator << frekvencije[2] << separator << frekvencije[3];
return ; }
        
        for(unsigned int i = 3; i<frekvencije.size(); i++)
            fn+=frekvencije[i];
            of << separator << frekvencije[0] ;
            of << separator << frekvencije[1] ;
            of << separator << frekvencije[2] ;
            of << separator << fn ;

  
//        of << "\n Frekvencije\n" ;
//        for(unsigned int i = 0; i<frekvencije.size(); i++)
//            of << separator << frekvencije[i] ;
    }

};

void Podaci::UradiPosao() {

  int br_corova = 0, br_grana = 0;
  br_corova = GetBrojCvorova();
  br_grana = GetBrojGrana();
  GetOdnosCvorovaGrana(br_corova,br_grana);
  GetUlazniStepenCvorova();
  GetIzlazniStepenCvorova();
  GetBrojInstrukcija();
  GetBrojInstrukcijaPoBloku();
  GetBrojInstrukcijaPoTeluPetlje(); 
  GetBrojCastova();
  GetBrojCastovaFpInt();
}



namespace {

class Petlje : public LoopPass {
public:
  static char ID; // Pass ID, replacement for typeid
  Petlje() : LoopPass(ID) {
    //   initializeFLoopUnrollPass(*PassRegistry::getPassRegistry());
  }

  /// A magic value for use with the Threshold parameter to indicate
  /// that the loop unroll should be performed regardless of how much
  /// code expansion would result.

  virtual bool runOnLoop(Loop *L, LPPassManager &LPM);
  //    virtual bool doInitialization(Loop *, LPPassManager &LPM);
  //    virtual bool doFinalization();

  /// This transformation requires natural loop information & requires that
  /// loop preheaders be inserted into the CFG...
  ///
  virtual void getAnalysisUsage(AnalysisUsage &AU) const {
//    AU.addRequiredID(LoopSimplifyID);
//    AU.addRequiredID(LCSSAID);
    AU.addRequired<LoopInfo>();

//    AU.addPreservedID(LCSSAID);
    AU.addPreserved<LoopInfo>();

    // FIXME: Loop unroll requires LCSSA. And LCSSA requires dom info.
    // If loop unroll does not preserve dom info then LCSSA pass on next
    // loop will receive invalid dom info.
    // For now, recreate dom info, if loop is unrolled.
    AU.addPreserved<DominatorTree>();
    AU.addPreserved<DominanceFrontier>();

    //ako se ovo doda onda postoje neke zavisnosti zbog kojih puca sve
    //t       AU.addRequired<ScalarEvolution>();
//    AU.addPreserved<ScalarEvolution>();

    //FIXME ovo je sumnjivo
  //  AU.addPreserved<AliasAnalysis>();
  }
};

std::vector<int> brInstrukcijaPoTeluPetlje;

bool Petlje::runOnLoop(Loop *L, LPPassManager &LPM) {
//  assert(L->isLCSSAForm());

//  LoopInfo *LI = &getAnalysis<LoopInfo>();

/*  BasicBlock *Header = L->getHeader();
  Function *ParentFunction = Header->getParent();


  for(llvm::Module::iterator f = mainModule->begin(), fend = mainModule->end(); f != fend; ++f){
    if (!(*f).isDeclaration()){
      llvm::LoopInfo& LI = &llvm::getAnalysis<llvm::LoopInfo>(*f);
*/      int bri = 0;


  std::vector<BasicBlock *> LoopBlocks = L->getBlocks();
for(unsigned i=0; i<LoopBlocks.size(); i++)
{
     BasicBlock* block = LoopBlocks[i];
      if (block->hasName()) {
        llvm::BasicBlock::iterator it = block->begin(), ite = block->end();
        for (; it != ite; it++) bri++;
      }   // end od if(bb->hasName())
}

//      for(llvm::LoopInfo::iterator l = LI->begin(), lend = LI->end(); l != lend; ++l){
//        llvm::Loop* loop = l;        
  
//      for(const llvm::Loop::iterator block = L->block_begin(), y = L->block_end(); block!= y; ++block) {

//      llvm::BasicBlock *bb = block;
      //Proveriti sta raditi sa blokovima koji nemaju ime
//      if (block->hasName()) {
//        llvm::BasicBlock::iterator it = block->begin(), ite = block->end();
//        for (; it != ite; it++) bri++;
//      }   // end od if(bb->hasName())


//}
  //    }
     brInstrukcijaPoTeluPetlje.push_back(bri);

  return true;
}

}



char Petlje::ID = 0;
static RegisterPass<Petlje> X("Petlje", "Petlje");

namespace lav {
Pass *createPetljePass() { return new Petlje(); }
}









int main(int argc, char **argv) {
  atexit(llvm_shutdown); // Call llvm_shutdown() on exit.
  lav::parseArguments(argc, argv);
  sys::PrintStackTraceOnErrorSignal();

  // Load the bytecode...
//  std::cout << std::endl << "Loading the bytecode..." << std::endl;
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
//  Passes.add(createLoopInfoPass()); // 
  Passes.add(llvm::createLoopSimplifyPass());
  Passes.add(lav::createPetljePass()); // 

  if (EnableInline)
    Passes.add(createAllInlinerPass(
        Threshold)); //Inline malo vece funkcije, parametar inlininga od 200 do
                     //milijardu, ako je bez argumenta postavljeno je na
                     //milijardu

  Passes.run(*mainModule);

  Podaci p(mainModule);
  p.UradiPosao();
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

int Podaci::GetBrojCastova() {
if(!BrojCastova) return 0;
  llvm::Module::iterator fnIt = mainModule->begin(), fn_ie = mainModule->end();
  int n = 0;

  for (; fnIt != fn_ie; ++fnIt) {
    if (fnIt->isDeclaration()) // && !fnIt->use_empty())
      continue;

    llvm::Function *f = fnIt;
    llvm::Function::iterator bbIt = f->begin(), bbIe = f->end();

    for (; bbIt != bbIe; ++bbIt) {

      llvm::BasicBlock *bb = bbIt;
      //Proveriti sta raditi sa blokovima koji nemaju ime
      if (bb->hasName()) {
        llvm::BasicBlock::iterator it = bb->begin(), ite = bb->end();
        for (; it != ite; it++){ 
          llvm::Instruction *ii = it;
          llvm::CastInst *instrCast = llvm::dyn_cast<llvm::CastInst>(ii);
          if(instrCast) n++;

        }
      }   // end od if(bb->hasName())
    }     //end od prolaska kroz kroz funkciju
  } // kraj prolaska kroz sve funkcije, trebalo bi da imamo sve cvorove

Upisi(n);
//Upisi(n, "GetBrojCastova");
return n;

}



int Podaci::GetBrojCastovaFpInt() {

  if(!BrojCastovaFpInt) return 0;

  llvm::Module::iterator fnIt = mainModule->begin(), fn_ie = mainModule->end();
  int n = 0;

  for (; fnIt != fn_ie; ++fnIt) {
    if (fnIt->isDeclaration()) // && !fnIt->use_empty())
      continue;

    llvm::Function *f = fnIt;
    llvm::Function::iterator bbIt = f->begin(), bbIe = f->end();

    for (; bbIt != bbIe; ++bbIt) {

      llvm::BasicBlock *bb = bbIt;
      //Proveriti sta raditi sa blokovima koji nemaju ime
      if (bb->hasName()) {
        llvm::BasicBlock::iterator it = bb->begin(), ite = bb->end();
        for (; it != ite; it++){ 
          llvm::Instruction *ii = it;
          llvm::CastInst *instrCast = llvm::dyn_cast<llvm::CastInst>(ii);
          if(instrCast && !instrCast->isIntegerCast()) n++;
        }
      }   // end od if(bb->hasName())
    }     //end od prolaska kroz kroz funkciju
  } // kraj prolaska kroz sve funkcije, trebalo bi da imamo sve cvorove

Upisi(n);
//Upisi(n, "GetBrojCastovaFpInt");

return n;

}



int Podaci::GetBrojGrana()
{
  if(!BrojGrana) return 0;
  int n = 0;
  llvm::Module::iterator fnIt = mainModule->begin(), fn_ie = mainModule->end();

  for (; fnIt != fn_ie; ++fnIt) {
    if (fnIt->isDeclaration()) // && !fnIt->use_empty())
      continue;

    llvm::Function *f = fnIt;
    llvm::Function::iterator bbIt = f->begin(), bbIe = f->end();

    for (; bbIt != bbIe; ++bbIt) {

      llvm::BasicBlock *bb = bbIt;
      //Proveriti sta raditi sa blokovima koji nemaju ime
      if (bb->hasName()) {
      llvm::succ_iterator it = llvm::succ_begin(bb),
                        ite = llvm::succ_end(bb);
        for (; it != ite; it++) n++;

      }   // end od if(bb->hasName())
    }     //end od prolaska kroz kroz funkciju
  } // kraj prolaska kroz sve funkcije, trebalo bi da imamo sve cvorove

Upisi(n);
//Upisi(n,"GetBrojGrana");
return n;
}


int Podaci::GetBrojCvorova()
{
 int num_blocks = 0;
  llvm::Module::iterator fnIt = mainModule->begin(), fn_ie = mainModule->end();

  for (; fnIt != fn_ie; ++fnIt) {
    if (fnIt->isDeclaration()) // && !fnIt->use_empty())
      continue;

    llvm::Function *f = fnIt;
    llvm::Function::iterator bbIt = f->begin(), bbIe = f->end();

    for (; bbIt != bbIe; ++bbIt) {
      llvm::BasicBlock *bb = bbIt;
      //Proveriti sta raditi sa blokovima koji nemaju ime
      if (bb->hasName()) {
        num_blocks++;
      }   // end od if(bb->hasName())
    }     //end od prolaska kroz kroz funkciju
  } // kraj prolaska kroz sve funkcije, trebalo bi da imamo sve cvorove

Upisi(num_blocks);
//    Upisi(num_blocks, "Broj cvorova");

return num_blocks;

}

static double GetAritmetickaSredina(std::vector<int>& niz) {
int s = 0;
for(unsigned int i=0; i<niz.size(); i++)
    s+=niz[i];
if(niz.size() == 0) return 0;
return (double)s/niz.size();
}

static double GetStandardnaDevijacija(std::vector<int>& niz) {
double A = GetAritmetickaSredina(niz);
double s = 0;

if(niz.size() == 0) return 0;

for(unsigned int i=0; i<niz.size(); i++)
    s+=(A - niz[i])*(A - niz[i]);

return sqrt((double)s/niz.size());
}
static double GetCoefOfVariation(std::vector<int>& niz) {
if(niz.size() == 0) return 0;
return GetStandardnaDevijacija(niz)/GetAritmetickaSredina(niz);
}


static int GetMax(std::vector<int>& niz) {
int max = niz[0];
for(unsigned int i=0; i<niz.size(); i++)
    if(max<niz[i]) max = niz[i];
return max;
}

static int GetMinNeqZero(std::vector<int>& niz) {
int min;
unsigned int i;
for(i=0; i<niz.size(); i++)
    if(niz[i]!=0) {min = niz[i]; break;}

for( ; i<niz.size(); i++)
    if(min>niz[i] && niz[i]!=0) min = niz[i];
return min;
}

static double GetProsek(std::vector<int>& niz) {
int sum = 0;
for(unsigned int i=0; i<niz.size(); i++)
    sum+= niz[i];
return (double)sum/(niz.size()>0 ? niz.size() : 1);
}


static void UzmiStatistike(std::vector<int> &niz, std::vector<double>& statistike) {
    if(niz.size() == 0) return;
//    for(unsigned int i=0; i<niz.size(); i++)
//        std::cout << niz[i] <<" ";
//    std::cout << std::endl;

    statistike.push_back(GetProsek(niz));
    statistike.push_back(GetMinNeqZero(niz));
    statistike.push_back(GetMax(niz));
    statistike.push_back(GetCoefOfVariation(niz));
    //statistike.push_back(GetEntropy(niz));

}

static void UzmiFrekvencije(std::vector<int> &niz, std::vector<double>& frekvencije) {
    if(niz.size() == 0) return; 

//    int maximum = GetMax(niz);
//    frekvencije.resize(maximum+1,0);
    frekvencije.resize(4,0);

//    std::cout <<"Ovo je niz" << std::endl;
//   for(unsigned int i=0; i<niz.size(); i++)
//        std::cout << niz[i] <<" ";
//    std::cout << std::endl;

   for(unsigned int i=0; i<niz.size(); i++) 
        frekvencije[niz[i]>2 ? 3:niz[i]]++;

//   for(unsigned int i=0; i<frekvencije.size(); i++)
//        std::cout << frekvencije[i] <<" ";
//   std::cout << std::endl;

   for(unsigned int i=0; i<frekvencije.size(); i++) 
    frekvencije[i] = frekvencije[i]/niz.size();

//   for(unsigned int i=0; i<frekvencije.size(); i++)
//        std::cout << frekvencije[i] <<" ";
//    std::cout << std::endl;

}

void Podaci::GetIzlazniStepenCvorova()
{
if(!IzlazniStepenCvorova) return;

std::vector<int> izlazniStepeni;

  llvm::Module::iterator fnIt = mainModule->begin(), fn_ie = mainModule->end();

  for (; fnIt != fn_ie; ++fnIt) {
    if (fnIt->isDeclaration()) // && !fnIt->use_empty())
      continue;

    llvm::Function *f = fnIt;
    llvm::Function::iterator bbIt = f->begin(), bbIe = f->end();

    for (; bbIt != bbIe; ++bbIt) {

      llvm::BasicBlock *bb = bbIt;
      //Proveriti sta raditi sa blokovima koji nemaju ime
      if (bb->hasName()) {
      int n = 0;
      llvm::succ_iterator it = llvm::succ_begin(bb),
                        ite = llvm::succ_end(bb);
        for (; it != ite; it++) n++;
      izlazniStepeni.push_back(n);  
      }   // end od if(bb->hasName())
    }     //end od prolaska kroz kroz funkciju
  } // kraj prolaska kroz sve funkcije, trebalo bi da imamo sve cvorove

//    of << "\nIzlazni stepeni ";
    Ocisti();
    UzmiStatistike(izlazniStepeni, statistike);
    OdstampajStatistike();

    UzmiFrekvencije(izlazniStepeni, frekvencije);
    OdstampajFrekvencije();

return ;

}



void Podaci::GetBrojInstrukcijaPoTeluPetlje()
{


/*  std::vector<llvm::Loop*> loops;
  // first gather all loop info
  for(llvm::Module::iterator f = mainModule->begin(), fend = mainModule->end(); f != fend; ++f){
    if (!(*f).isDeclaration()){
      llvm::LoopInfo& LI = &llvm::getAnalysis<llvm::LoopInfo>(*f);
      int bri = 0;
      for(llvm::LoopInfo::iterator l = LI.begin(), lend = LI.end(); l != lend; ++l){
          bri++;

//for(Loop::block_iterator block = loop.block_begin(), y = loop.block_end(); block!= y; ++block)
      }
     brojInstrukcija.push_back(bri);
    }
  }
*/
//    of << "\nBroj instrukcija po petlji ";
    Ocisti();
    UzmiStatistike(brInstrukcijaPoTeluPetlje, statistike);
    OdstampajStatistike();
}



void Podaci::GetBrojInstrukcijaPoBloku()
{
  if(!BrojInstrukcijaPoBloku) return;
  std::vector<int> brojInstrukcija;

  llvm::Module::iterator fnIt = mainModule->begin(), fn_ie = mainModule->end();

  for (; fnIt != fn_ie; ++fnIt) {
    if (fnIt->isDeclaration()) // && !fnIt->use_empty())
      continue;

    llvm::Function *f = fnIt;
    llvm::Function::iterator bbIt = f->begin(), bbIe = f->end();

    for (; bbIt != bbIe; ++bbIt) {

      llvm::BasicBlock *bb = bbIt;
      //Proveriti sta raditi sa blokovima koji nemaju ime
      if (bb->hasName()) {
      int n = 0;
        llvm::BasicBlock::iterator it = bb->begin(), ite = bb->end();
        for (; it != ite; it++) n++;
      brojInstrukcija.push_back(n);  
      }   // end od if(bb->hasName())
    }     //end od prolaska kroz kroz funkciju
  } // kraj prolaska kroz sve funkcije, trebalo bi da imamo sve cvorove

//    of << "\nBroj instrukcija po bloku ";
    Ocisti();
    UzmiStatistike(brojInstrukcija, statistike);
    OdstampajStatistike();

return ;

}

int Podaci::GetBrojInstrukcija()
{

  if(!BrojInstrukcija) return 0;

  llvm::Module::iterator fnIt = mainModule->begin(), fn_ie = mainModule->end();
  int n = 0;

  for (; fnIt != fn_ie; ++fnIt) {
    if (fnIt->isDeclaration()) // && !fnIt->use_empty())
      continue;

    llvm::Function *f = fnIt;
    llvm::Function::iterator bbIt = f->begin(), bbIe = f->end();

    for (; bbIt != bbIe; ++bbIt) {

      llvm::BasicBlock *bb = bbIt;
      //Proveriti sta raditi sa blokovima koji nemaju ime
      if (bb->hasName()) {
        llvm::BasicBlock::iterator it = bb->begin(), ite = bb->end();
        for (; it != ite; it++) n++;
      }   // end od if(bb->hasName())
    }     //end od prolaska kroz kroz funkciju
  } // kraj prolaska kroz sve funkcije, trebalo bi da imamo sve cvorove

Upisi(n);

//Upisi(n,"Broj instrukcija");

return n;

}


void Podaci::GetUlazniStepenCvorova()
{

if(!UlazniStepenCvorova) return;
std::vector<int> ulazniStepeni;

  llvm::Module::iterator fnIt = mainModule->begin(), fn_ie = mainModule->end();

  for (; fnIt != fn_ie; ++fnIt) {
    if (fnIt->isDeclaration()) // && !fnIt->use_empty())
      continue;

    llvm::Function *f = fnIt;
    llvm::Function::iterator bbIt = f->begin(), bbIe = f->end();

    for (; bbIt != bbIe; ++bbIt) {

      llvm::BasicBlock *bb = bbIt;
      //Proveriti sta raditi sa blokovima koji nemaju ime
      if (bb->hasName()) {
      int n = 0;
      llvm::pred_iterator it = llvm::pred_begin(bb),
                        ite = llvm::pred_end(bb);
        for (; it != ite; it++) n++;
      ulazniStepeni.push_back(n);  
      }   // end od if(bb->hasName())
    }     //end od prolaska kroz kroz funkciju
  } // kraj prolaska kroz sve funkcije, trebalo bi da imamo sve cvorove

//    of << "\nUlazni stepeni cvorova ";
    Ocisti();
    UzmiStatistike(ulazniStepeni, statistike);
    OdstampajStatistike();

    UzmiFrekvencije(ulazniStepeni, frekvencije);
    OdstampajFrekvencije();

return ;

}


static int OpenFileWithExtension(std::ofstream &of, std::string Ext) {
  std::string s = OutputFolder + "/" + InputFileName+"."+Ext;
  of.open(s.c_str());
  if (!of)
    exit_error("OpenFileAddExtension::Could not open " + s + " file!");

  return 1;  
}


static int OpenOutputFile(std::ofstream &of, std::string Ext) {
  std::string s = OutputFolder + "/" + OutputFileName; //+"."+Ext;
  of.open(s.c_str(), std::ios::app);
  if (!of)
    exit_error("OpenOutputFile::Could not open " + s + " file!");
  of << "\n" << InputFileName ;
  return 1;
}



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

/*
static void DumpCFG(Module *mainModule) {
  std::ofstream nodes;
  std::ofstream cfg;

  //    KorpusZaKlasifikaciju(cfg, nodes);
  OpenCFGAndNodesFiles(cfg, nodes);
  DumpCFG(mainModule, cfg, nodes);

  cfg.close();
  nodes.close();
}*/

///////////////////////////////////////////////////////////////////


