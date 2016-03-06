//===-- test.cpp ----------------------------------------------------------===//
//
//                     The LAV Software Verification Tool
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

#include <iostream>
#include <fstream>

#include "llvm/Support/CommandLine.h"
#include "lav/Test/test.h"
#include "lav/Misc/misc.h"

#define MAX_BUF 1000

namespace {

  llvm::cl::opt<std::string>
  _OutputFolder("output-folder", 
               llvm::cl::desc("testLAV --- Name of output folder (default = Output)"),
               llvm::cl::init("Output"));

  llvm::cl::opt<bool>
  testMode("test", 
               llvm::cl::desc("testLAV --- test with existing results (default = false)"),
               llvm::cl::init(false));

  llvm::cl::opt<std::string>
  directory("directory",
                llvm::cl::desc("testLAV --- files from this directory will be tested/generated"),
                 llvm::cl::init("-"));

  llvm::cl::opt<std::string>
  testFile("test-file",
                llvm::cl::desc("testLAV --- file that contains LAV options that will be used for testing/generating (default = \"test\")"),
                 llvm::cl::init("test"));

  llvm::cl::opt<std::string>
  lavOptions("lav-options",
                llvm::cl::desc("testLAV --- additional Lav options that will be applied to default ones  (default = \"\")"),
                 llvm::cl::init(""));

  llvm::cl::opt<std::string>
  logFile("log-file",
                llvm::cl::desc("testLAV --- file that will contain LAV results (default = \"results.log\")"),
                 llvm::cl::init("results.log"));

  llvm::cl::opt<bool>
  model("model",
                llvm::cl::desc("testLAV --- check generated model (default = false)"),
                 llvm::cl::init(false));

  llvm::cl::opt<std::string>
  modelFile("model-file",
                llvm::cl::desc("testLAV --- model file name (default = \"model\")"),
                 llvm::cl::init("model"));

  llvm::cl::opt<bool>
  sameOptions("same-options", 
               llvm::cl::desc("testLAV --- same options (given in a testfile) for all programs (default = false)"),
               llvm::cl::init(false));
}

namespace lav {

std::string OutputFolder() {
return _OutputFolder;
}

bool TestMode() 
{return testMode;}

bool SameOptions() 
{return sameOptions;}

bool Model() 
{return model;}

std::string ModelFile()
{return modelFile;}

std::string LogFile()
{return logFile;} 

std::string Directory()
{return directory;}

std::string TestFile()
{return testFile;}

std::string LavOptions()
{return lavOptions;}

void ReadDirectoryItems(DIR *dir, const char* dname, FILE* fout);
void VerFileName(const char* directoryName, char* log_file);
void BugFileName(const char* directoryName, char* log_file);
void LogFileName(const char* directoryName, char* log_file);
FILE* OpenLogFile(const char* log_file = NULL);
void PrintHeader(FILE* flog);
void WriteTimeIntoLog(const char* log_file);
void LlvmGcc(const char* fileName);
void MakeObjFileName(char* objname, char* fileName);
void AddDirectoryToFileName(const char* directoryName, const char* fileName, char* file);
void CallLlvmGcc(const char* directoryName, char* obj, char* fileName);
void MakeExpectedOutputName(char* outputName, char* fileName, const char* directoryName);
void changeToTxt(char* outputName);
void MakeLavOutputName(char* lavoutputname, const char* name);
void CopyOutputFile(const char* directoryName, char* fileName, char* name);
void CallLav(char* command, char* objname, char* log_file, const char* s, const char* additionalOptions, int& verified, int& bugs, char* ver, char* bug, char* fileName);
bool CheckFiles(FILE* ExpectedOutput, FILE* LavOutput, const char* filename, const char* log_file);
FILE* OpenForReading(const char* filename, const char* file, const char* log_file, const char* cf);
bool CheckModel(const char* directoryName, char* objname, char* filename, const char* log_file, const char* modelFile);
bool Check(const char* directoryName, char* objname, char* filename, const char* log_file);
DIR* OpenDir(const char* directoryName);
FILE* MakeProgramsList(const char* directoryName);
bool IsCommand(char* command);
bool GetFileName(char* cname, FILE* test);
bool GetCommand(char* command, FILE* test);
FILE* OpenTestFile(const char* directoryName, const char* testFileName);
void osisaj(const char* fileName, char* file1, const char* directoryName, char* dir1);
void PrintSummary(FILE* f, int verified, int bugs, int failure);
void WriteIntoLog(const char* log_file,std::string s);

static std::string log_file_name="log"; 

void termination_handler(int signum)
{
std::string message = "\n\n------------------------------------\nLAVtest received termination signal\n------------------------------------ \n\n"; 
std::cerr << message <<std::endl;
WriteIntoLog(log_file_name.c_str(), message);
//trebalo bi jos pobrisati stvari
exit(1);
}

void ReadDirectoryItems(DIR *dir, const char* dname, FILE* fout)
{
    struct dirent* dent;
    struct stat st;
    char fn[FILENAME_MAX];
    int len = strlen(dname);
    strcpy(fn, dname);
    fn[len++] = '/';

    while(1)
    {
    dent = readdir(dir);
    if (!dent) return;
    if(!strcmp(dent->d_name, "."))  continue;
    if(!strcmp(dent->d_name, "..")) continue;
    if(dent->d_name[0] == '.')     continue;

    strncpy(fn + len, dent->d_name, FILENAME_MAX - len);
    if (lstat(fn, &st) == -1) {
            perror(std::string("Can't stat ")+fn);
            continue;
    }
 
    /* don't follow symlink  */
    if (S_ISLNK(st.st_mode))
            continue;
 
    /* will be false for symlinked dirs */
    if (S_ISDIR(st.st_mode)) {
    DIR* dir1;
    if (!(dir1 = opendir(fn))) {
            printf("can't open %s", fn);
            continue;
    }
            /* recursively follow dirs */
     ReadDirectoryItems(dir1, fn, fout);
     closedir(dir1);
    }

    if(!strstr(dent->d_name, ".c")) continue;
    if(strstr(dent->d_name, ".c~")) continue;
    if(strstr(dent->d_name, ".cf")) continue;
    fprintf(fout, "%s\n", fn);
    }
}





void VerFileName(const char* directoryName, char* log_file)
{
  strcpy(log_file, directoryName);
  strcat(log_file, "/verified.log");
}

void BugFileName(const char* directoryName, char* log_file)
{
  strcpy(log_file, directoryName);
  strcat(log_file, "/bugs.log");
}


void LogFileName(const char* directoryName, char* log_file)
{
  strcpy(log_file, directoryName);
  strcat(log_file, "/");
  strcat(log_file, LogFile().c_str());
}


FILE* OpenLogFile(const char* log_file)
{
  FILE* flog = fopen(log_file, "a");
  if(flog==NULL) {  exit_error((std::string)"Could not open log file " + (std::string)log_file);}
  return flog;
}


void PrintHeader(FILE* flog)
{
  //Upisivanje zaglavlja u log datoteku
  time_t curtime;
  struct tm *loctime;

  /* Get the current time. */
  curtime = time (NULL);
  /* Convert it to local time representation. */
  loctime = localtime (&curtime);

  /* Print out the date and time in the standard format. */
  fputs ("\n*************************************************************************************\n", flog);
  fputs ("*************************************************************************************\n", flog);
  fputs ("************-------------------------------------------------------------************\n", flog);
  fputs ("************-------------------------------------------------------------************\n", flog);
  fputs ("************             ", flog);  
  fputs (asctime (loctime), flog);
  fputs ("************-------------------------------------------------------------************\n", flog);
  fputs ("************-------------------------------------------------------------************\n", flog);
  fputs ("*************************************************************************************\n", flog);
  fputs ("*************************************************************************************\n\n\n\n", flog);

}

void WriteTimeIntoLog(const char* log_file)
{
  FILE* flog = OpenLogFile(log_file);
  PrintHeader(flog);
  fclose(flog);
}

void WriteIntoLog(const char* log_file,std::string s)
{
  FILE* flog = OpenLogFile(log_file);
  fprintf(flog, "%s\n", s.c_str());
  fclose(flog);
}


void LlvmGcc(const char* fileName)
{
  char llvmgccname[MAX_BUF];
  int sys;

  /*Pravi se .o fajl*/
//  strcpy(llvmgccname, "llvm-gcc --emit-llvm -c -g ");
  strcpy(llvmgccname, "clang -emit-llvm -c -g ");
  strcat(llvmgccname, fileName);
  strcat(llvmgccname, " 2>>/dev/null ");
  sys = system(llvmgccname);
}



void changeToObj(char* objname)
{
    int len = strlen(objname);
    objname[len-1] = 'o';
}

void MakeObjFileName(char* objname, char* fileName)
{
    /*Napravi ime objektnog fajla*/
    strcpy(objname, fileName);
    changeToObj(objname);
}

void AddDirectoryToFileName(const char* directoryName, const char* fileName, char* file)
{
    if(strlen(directoryName)>0)
    {
    strcpy(file, directoryName);
    strcat(file, "/");
    strcat(file, fileName);
    }
    else strcpy(file,fileName);
}

void CallLlvmGcc(const char* directoryName, char* obj, char* fileName)
{
    char file[MAX_BUF];
    AddDirectoryToFileName(directoryName, fileName, file);
    LlvmGcc(file);
}


void MakeExpectedOutputName(char* outputName, char* fileName, const char* directoryName)
{
    /*Napravi ime sa putanjom txt fajla*/
    AddDirectoryToFileName(directoryName, fileName, outputName);
    changeToTxt(outputName);
}

void changeToTxt(char* outputName)
{
    int len = strlen(outputName);
    outputName[len-1] = 't';
    strcat(outputName, "xt");
}

void MakeLavOutputName(char* lavoutputname, const char* name)
{
    strcpy(lavoutputname, (OutputFolder()+"/").c_str());
    int p = 7, k;
    unsigned j;
    for( k = strlen(name)-1; k>=0; k--)
    if(name[k] == '/') break;

    for( j = k+1; j<strlen(name); j++)
    lavoutputname[p++] = name[j];
    
    lavoutputname[p] =  '\0';
}



void CopyOutputFile(const char* directoryName, char* fileName, char* name)
{
  char lavoutputname[MAX_BUF];
  MakeLavOutputName(lavoutputname, fileName);
  changeToTxt(lavoutputname);  
  changeToTxt(name);  
  CopyFile(lavoutputname, name);
}

void CallLav(char* command, char* objname, char* log_file, const char* s, const char* additionalOptions, int& verified, int& bugs, char* ver, char* bug, char* fileName)
{

    int sys;
    int length=strlen(command);
    if(length==0)
        strcpy(command, "time LAV ");

    strcat(command, additionalOptions);        
    strcat(command, " ");
    strcat(command, objname);
    strcat(command, " >/dev/null 2>>");
    strcat(command, log_file);


    printf("\n-----------------------------------------------------------\n"); 
    printf("%s\n%s ................. ", command, s);
    fflush(stdout);

    sys = system(command);

    if(sys != 0) {
    bugs++; 
    WriteIntoLog(log_file, (std::string)"Verification failed:\n" + command + "\nfilename " +fileName +"\n\n\n" );
    WriteIntoLog(bug, (std::string)"Verification failed:\n" + command + "\nfilename " +fileName +"\n\n\n" );
    }
    else {
    verified++; 
    WriteIntoLog(log_file, (std::string)"Verification successfull:\n" + command + "\nfilename " +fileName +"\n\n\n" );
    WriteIntoLog(ver, (std::string)"Verification successfull:\n" + command + "\nfilename " +fileName +"\n\n\n" );
    }

    WriteIntoLog(log_file, (std::string)"command: " + command + "\nfilename " +fileName +"\n\n\n" );
}

int mystrcmp(char* expected, char* lavout)
{
char* cutExpected, *cutLavout;

if(strstr(expected,"VERIFICATION")) {
    return strcmp(expected, lavout);
}
if((cutExpected = strstr(expected,"Assembly line"))) 
{
    cutLavout = strstr(lavout,"Assembly line");
    if(cutLavout == NULL) return -1;
    *cutExpected = '\0';
    *cutLavout = '\0';
    int sc = strcmp(expected, lavout);
    *cutExpected = 'A';
    *cutLavout = 'A';
    return sc;
}
return strcmp(expected, lavout);
}

bool CheckFiles(FILE* ExpectedOutput, FILE* LavOutput, const char* filename, const char* log_file)
{
    char* s1, *s2;
    char str1[MAX_BUF]; 
    char str2[MAX_BUF]; 

    while(1)
    {
    s1 = fgets(str1, MAX_BUF, ExpectedOutput);
    s2 = fgets(str2, MAX_BUF, LavOutput);
    if((s1 == NULL) && (s2==NULL)) break;
    if((s1 != NULL) && (s2 != NULL) && (!mystrcmp(str1,str2))) {continue;}
    if(s1==NULL || s2 == NULL || mystrcmp(str1,str2))
        {
        if(s1==NULL) {
            perror((std::string)"CheckFiles 1: File name "+ filename + "\nExpectedOutput\n \nLavOutput\n" + str2 + "\n", log_file);
            }
        else if(s2==NULL) {
            perror((std::string)"CheckFiles 2: File name "+ filename + "\nExpectedOutput\n"+str1+" \nLavOutput\n", log_file);
            }
        else {
            perror((std::string)"CheckFiles 3: File name "+ filename + "\nExpectedOutput\n"+str1+" \nLavOutput\n "+ str2 + "\n", log_file);
        }
        perror((std::string)"CheckFiles 4: File name: "+ filename + "\n", log_file);
        return false;
        }
    }
   return true;
}

FILE* OpenForReading(const char* filename, const char* file, const char* log_file, const char* callingFunction)
{
    FILE* f = fopen(filename, "r");
    if(f == NULL) {
            fprintf(stderr, "Error opening file: %s\n", strerror( errno ) );
            perror(std::string("Could not open file: ") + std::string(filename) + " from function " + callingFunction, log_file);
            }
    return f;
}

bool CheckModel(const char* directoryName, char* objname, char* filename, const char* log_file, const char* modelFile)
{
    char s[MAX_BUF];
    char expectedModel[MAX_BUF];
    char lavoutputmodel[MAX_BUF];
    char modelFileWithDir[MAX_BUF];

    AddDirectoryToFileName(directoryName, modelFile, modelFileWithDir);

    FILE* modeli = OpenForReading(modelFileWithDir, filename, log_file, "CheckModel 1");
    if((modeli == NULL))    return false;

    while(fgets(s, MAX_BUF, modeli)!=NULL)
       {
       int len = strlen(s);
        if(len>0) s[len-1]='\0'; //izbrise znak za kraj
        if(!strcmp(s,filename)) break;
        }
        
    if(fgets(s, MAX_BUF, modeli) == NULL) {    
        perror((std::string)"Could not read model file name for " + filename + " in " + modelFile +" file \n");
        fclose(modeli); 
        return false; }

    fclose(modeli);

    int len = strlen(s);
    if(len>0) s[len-1]='\0'; //izbrise znak za novi red

    AddDirectoryToFileName(directoryName, s, expectedModel);
    MakeLavOutputName(lavoutputmodel, s);

    FILE* LavOutput = OpenForReading(lavoutputmodel, filename, log_file, "CheckModel 2");
    if((LavOutput == NULL)) return false;

    FILE* ExpectedOutput = OpenForReading(expectedModel , filename, log_file, "CheckModel 3");
    if((ExpectedOutput == NULL)) {fclose(LavOutput); return false;}

    bool b = CheckFiles(ExpectedOutput, LavOutput, filename, log_file);
    Delete(lavoutputmodel);
    fclose(LavOutput);
    fclose(ExpectedOutput);
   return b;
}


bool Check(const char* directoryName, char* objname, char* filename, const char* log_file)
{
    FILE* LavOutput;
    FILE* ExpectedOutput;

    char name[MAX_BUF]; 
    char lavoutputname[MAX_BUF]; 


    MakeExpectedOutputName(name, filename, directoryName);
    MakeLavOutputName(lavoutputname, name);

    /*Uporedi izlaz lava sa datim  izlazom*/
    LavOutput = OpenForReading(lavoutputname, filename, log_file, "Check 1"); 
    if((LavOutput == NULL)) { Delete(objname); return false;}


    ExpectedOutput = OpenForReading(name, filename, log_file, "Check 2");
    if((ExpectedOutput == NULL))
    {
    Delete(objname);
    Delete(lavoutputname);
    fclose(LavOutput);
    return false;
    }

    bool b = CheckFiles(ExpectedOutput, LavOutput, filename, log_file);

    /*Zatvaraju se datoteke*/
    fclose(LavOutput);
    fclose(ExpectedOutput);
    Delete(lavoutputname);
    return b;
}


DIR* OpenDir(const char* directoryName)
{
    DIR* dir = opendir(directoryName);
    if(dir==NULL) {
    exit_error(std::string("Could not open directory ") + (std::string)directoryName);
    }
    return dir;
} 


FILE* MakeProgramsList(const char* directoryName)
{
    DIR *dir;
    FILE* fout;
    std::string plist = OutputFolder()+"/ProgramsList";
    fout = fopen(plist.c_str(),"w");
    if(fout == NULL) {exit_error("Nije uspleo otvaranje ProgramList datoteke za pisanje"); }
    printf("\n-----------------------------------------------------------\n"); 
    printf("%s\n", directoryName);

    dir = OpenDir(directoryName);
    ReadDirectoryItems(dir, directoryName, fout);
    fclose(fout);

    fout = fopen(plist.c_str(),"r");
    if(fout == NULL) {exit_error("Nije uspleo otvaranje ProgramList datoteke za citanje"); }

    return fout;
}


bool IsCommand(char* command)
{
    //komanda je barem "LAV ime" sto je bar 5 karaktera
    if(strlen(command) < 5) { 
      return false;}
    //da li je komentar
    if((command[0] == '/') && (command[1] == '/')) {
      return false;}
    //moze da pocne sa time ili sa lav to je u redu
    if(! (((command[0]=='t') && (command[1] == 'i')) || ((command[0]=='L') && (command[1] == 'A')))) {
      return false;}
      
    int length=strlen(command);
    if(length>0)
    {
        if(command[length-1] == '\n') command[length-1] = ' ';//brise se znak za novi red
        command[length] = '\0';
    }
   
    return true;
}

bool GetFileName(char* cname, FILE* test)
{
    //ako je ucitana komanda ispravna, onda se ucitava ime c fajla, ono je u sledecem redu
    if(fgets(cname, MAX_BUF, test)==NULL) 
    { 
    perror("Could not read from c file \n");
    return false; 
    }

    if(feof(test)) {return false;}
    return true; 
}

bool GetCommand(char* command, FILE* test)
{
    char* fg = fgets(command, MAX_BUF, test);
    if(fg == NULL) return false;
    if(feof(test)) { return false;}
    return true;
}

FILE* OpenTestFile(const char* directoryName, const char* testFileName)
{
  FILE* test;

  test = fopen(testFileName, "r");
  if(test == NULL) 
  {
  char test_file[MAX_BUF];
  strcpy(test_file, directoryName);
  strcat(test_file, "/");
  strcat(test_file, testFileName);
  test = fopen(test_file, "r");
  }
  return test;
}

//ako se je ime fajla proba/file.c onda ce file1 da bude jednako file a na 
//dir1 ce se nakon direcotryName-a nakaciti i proba
void osisaj(const char* fileName, char* file1, const char* directoryName, char* dir1)
{
    int i, k, m;
    strcpy(file1, fileName);
    strcpy(dir1,directoryName); 
    for(i=strlen(fileName) -1; i>=0 && fileName[i]!='/'; i--);
    if(i>0)
        {i++;
        for(m=0, k=i; k<=(signed)(strlen(fileName)-1); k++, m++) file1[m] = fileName[k];
        file1[m]='\0';
        if(strlen(dir1)>0)
                if(dir1[strlen(dir1)-1]!='/') {int p = strlen(dir1)-1; dir1[++p]='/'; dir1[++p]='\0';}
        for(m=strlen(dir1), k=0; k<i;k++, m++) dir1[m] = fileName[k];
        dir1[m]='\0';
        }

}
const char* generating = "Generating";
const char* checking = "Checking";

void PrintSummary(FILE* f, int verified, int bugs, int failure)
{
fprintf(f, "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"); 
fprintf(f, "Number of programs without bugs: %d \n", verified); 
fprintf(f, "Number of programs with bugs: %d \n", bugs); 
fprintf(f, "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n\n\n"); 


if(failure>0)
{
fprintf(f, "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"); 
fprintf(f, "Number of failures:  !!!!!!!!!! %d !!!!!!!!\n", failure); 
fprintf(f, "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"); 
}
else
{
fprintf(f, "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"); 
fprintf(f, "No failures\n"); 
fprintf(f, "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"); 
}

if(!failure)
  fprintf(f, "\n OK \n");
else  fprintf(f, "\n\n JAO JAO JAO!!!!! \n\n\n");

  fprintf(f, "\n-----------------------------------------------------------\n"); 

}


void Test(const char* directoryName, const char* testFileName, const char* lavOptions, bool check, bool sameOptions, const char* modelFile, bool model)
{
    FILE* flog = NULL;
    FILE* fout = NULL;
    char objname[MAX_BUF];
    char cname[MAX_BUF];
    char command[MAX_BUF] = {'\0'};
    char command1[MAX_BUF] = {'\0'};

    char log_file[MAX_BUF];  
    char ver_file[MAX_BUF];
    char bug_file[MAX_BUF];
    int bugs = 0, verified = 0, failure = 0;


    char* fileName;
    char file1[MAX_BUF];
    char dir1[MAX_BUF];

    const char* job;
    if(check) job = checking;
    else job = generating;


    //proveravamo da li postoji testfile
    FILE* test = OpenTestFile(directoryName, testFileName);

    //ako ne postoji skupimo sva imena programa u jednu datoteku
    if(test==NULL || (sameOptions == true))
    {
    MakeDirectory(OutputFolder());
    fout = MakeProgramsList(directoryName);
    }

    if(sameOptions)
    {
    if(test==NULL) sameOptions = false;
    else {
      if(!GetCommand(command1, test)) {exit_error("Greska u test file-u");}
      else {
        if(!IsCommand(command1)) {exit_error("Greska u test file-u");}
        command1[strlen(command1)-1] = '\0'; 
        printf("Glavna komanda je: %s\n", command1);
        fclose(test);
        test = NULL;
        }
      }
    }

    VerFileName(directoryName, ver_file);
    BugFileName(directoryName, bug_file);
    LogFileName(directoryName, log_file);

    log_file_name = (std::string)log_file;
    
    WriteTimeIntoLog(log_file);
    WriteTimeIntoLog(ver_file);
    WriteTimeIntoLog(bug_file);


    for (;;)
    {


    if(test==NULL) //znaci da nemamo testfile nego obilazmo direktorijum
    {
        if(fgets(fileName, MAX_BUF, fout) == NULL) break;
        fileName[strlen(fileName)-1] = '\0'; 
        printf("%s\n", fileName);
        if(!sameOptions) command[0] = '\0';
        else strcpy(command, command1);    
        osisaj(fileName, file1, "", dir1);
    }
    else //idemo po testfile-u 
    {
        if(!GetCommand(command, test)) break;
        if(!IsCommand(command)) continue;
        if(!GetFileName(cname, test)) break;
        fileName = cname;
        fileName[strlen(cname)-1] = '\0';
        osisaj(fileName, file1, directoryName, dir1);
    }

    CallLlvmGcc(dir1, objname, file1);
    MakeObjFileName(objname, file1);
    CallLav(command, objname, log_file, job, lavOptions, verified, bugs, ver_file, bug_file, fileName);

    if(check)
    {
        if(!Check(dir1, objname, file1, log_file)) 
        {
            failure++;
            continue;
        }
        if(model)
        {
            if(!CheckModel(dir1, objname, file1, log_file, modelFile))
            {
                failure++;
                continue;            
            }
        }
    }
    else //znaci samo generisemo
    {
        CopyOutputFile(directoryName, file1, fileName);
    }   
    /*Upisi u log rezultat*/
    printf("OK\n"); 

    WriteIntoLog(log_file, (std::string)"OK: "+command+ " \n\n\n\n\n" );

    Delete(objname);
    printf("-----------------------------------------------------------\n");  
    }
    
    if(test==NULL) fclose(fout);
    else fclose(test);
    flog = OpenLogFile(log_file);
    PrintSummary(flog, verified, bugs, failure);
    fclose(flog);
    
//    PrintSummary(stderr, verified, bugs, failure);
    if(!failure)
      fprintf(stderr, "\n OK \n");
    else  fprintf(stderr, "\n\n JAO JAO JAO!!!!! \n\n\n");

}

}

