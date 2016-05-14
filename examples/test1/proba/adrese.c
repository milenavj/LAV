#include<stdlib.h>

int adresa(int k)
{
int* p;
int a = k;
int c = k++;
p = &a;
*p = *p/a;	/*SAFE, SAFE, UNSAFE*/
p = &c;
return a;
}


int izracunaj(int a)
{
int c;
c = 3/a;	/*UNSAFE*/
return c;
}
	
int funpok(int* p)
{
*p = 3/(*p);	/*UNSAFE, UNSAFE, UNSAFE*/
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
b = izracunaj(*p);	/*SAFE*/

b = adresa(c); 		/*SAFE*/

p=&c;
c = a/(b-1); 		/*FLAWED*/
p = &a;
*p = 5;  		/*SAFE*/

c = a/(a-5); /*FLAWED*/

c = funpok(p); /*SAFE, a = 0, c = 0*/

c = (b+3)/a; /*FLAWED*/
}



