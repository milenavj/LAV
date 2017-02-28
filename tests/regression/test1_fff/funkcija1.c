#include<stdlib.h>
int izracunaj(int a)
{
int c;
c = 3/a;
return c;

/*
if(c>0)
	return c;
if(c == 0) return (c+1);
return (-c);
*/
}


int main()
{

int a = 11;
int b = 0;
int c = 5;
	
a = izracunaj(4); /*SAFE*/
b = c/a; 	/*FLAWED*/


c = izracunaj(c); /*SAFE*/
b = b/c;	/*FLAWED*/

c = izracunaj(b); /*SAFE*/
c = izracunaj(c); /*SAFE*/

c = izracunaj(0); /*FLAWED*/

if(c > 0) a = a/c; /*UNREACHABLE ???*/
else b = b/c;	   /*SAFE ???*/

c = izracunaj(a); /*FLAWED*/
return 0;
}



