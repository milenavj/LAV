#include<stdlib.h>

int izracunaj(int a)
{
int c;
c = 3/a;	/*UNSAFE*/
return c;
}

int funpok(int* p)
{
int c;
*p = 3/(*p); /*UNSAFE, UNSAFE, UNSAFE*/
return *p;	/*UNSAFE*/
}

int main()
{

int a = 11;
int b = 0;
int c = 5;
int* p;

p = &a;
if(c>0) c++;
b = izracunaj(*p);	/*SAFE, SAFE*/

c = a/b; /*FLAWED*/
*p = 5;  /*SAFE*/

c = a/(a-5); /*FLAWED*/

c = funpok(p); /*SAFE x 4, a = 0, c = 0*/

c = (b+3)/a; /*FLAWED*/
}



