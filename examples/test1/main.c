#include<string.h>
#include<stdlib.h>
int glob1;
int glob2;
int niz[5];

int main(int argc, char** argv)
{

if(argc>5) atoi(argv[3]); /*SAFE*/

if(argc<4) exit(1);

glob1 = atoi(argv[3]);  /*SAFE*/
niz[glob1] = 3;        /*UNSAFE*/

glob2 = 5 / (glob1-3); /*UNSAFE*/

niz[glob2] = 3;        /*UNSAFE*/

glob1 = glob2 - glob2;
glob2 = glob2/glob1; /*FLAWED*/

}