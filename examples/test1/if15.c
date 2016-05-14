#define konstanta 4
#include <stdio.h>

/*int glob=5;*/

int funkcija()
{ return 3; }

main()
{
int a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14;
int b;

//a0 = funkcija();
/*a0 = 3;*/

if(a0>0) a1 = 1; else a1 = -1;
if(a1>0) a2 = -1; else a2 = 1;
if(a2>0) a3 = 1; else a3 = -1;
if(a3>0) a4 = -1; else a4 = 1;
if(a4>0) a5 = 1; else a5 = -1;
if(a5>0) a6 = -1; else a6 = 1;
if(a6>0) a7 = 1; else a7 = -1;
if(a7>0) a8 = -1; else a8 = 1;
if(a8>0) a9 = 1; else a9 = -1;
if(a9>0) a10 = -1; else a10 = 1;
if(a10>0) a11 = 1; else a11 = -1;
if(a11>0) a12 = -1; else a12 = 1;
if(a12>0) a13 = 1; else a13 = -1;
if(a13>0) a14 = -1; else a14 = 1;

b = a0/(a1+a2+a3+a4+a5+a6+a7+a8+a9+a10+a11+a12+a13+a14); /*FLAWED*/

a11 = a10+2;
printf("%d%d\n",b,a11);

if (b>0) printf("%d%d\n",b,a11);
else printf("%d%d\n",b,a12);

b = a0/(a1+a2+a3+a4+a5+a6+a7+a8+a9+a10+a11+a12+a13+a14-2); /*FLAWED*/
printf("%d%d\n",b,a11);


int x;

x = 2;
x = x + 3;
x = x/3; /*SAFE*/



/*
scanf("%d",&a0);
scanf("%d",&b);

if(a0>0) a1 = 1; else a1 = -1;
if(a1>0) a2 = -1; else a2 = 1;
if(a2>0) a3 = 1; else a3 = -1;
if(a3>0) a4 = -1; else a4 = 1;
if(a4>0) a5 = 1; else a5 = -1;
if(a5>0) a6 = -1; else a6 = 1;
if(a6>0) a7 = 1; else a7 = -1;
if(a7>0) a8 = -1; else a8 = 1;
if(a8>0) a9 = 1; else a9 = -1;
if(a9>0) a10 = -1; else a10 = 1;
if(a10>0) a11 = 1; else a11 = -1;
if(a11>0) a12 = -1; else a12 = 1;
if(a12>0) a13 = 1; else a13 = -1;
if(a13>0) a14 = -1; else a14 = 1;

b = a0/(a1+a2+a3+a4+a5+a6+a7+a8+a9+a10+a11+a12+a13+a14);

printf("%d%d\n",b,a11);

a11 = a10+2;
printf("%d%d\n",b,a11);

if (b>0) printf("%d%d\n",b,a11);
else printf("%d%d\n",b,a12);

b = a0/(a1+a2+a3+a4+a5+a6+a7+a8+a9+a10+a11+a12+a13+a14-2);
printf("%d%d\n",b,a11);
*/
}
