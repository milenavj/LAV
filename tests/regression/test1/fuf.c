#include<stdlib.h>

int povecaj(int l)
{
return ++l;
}


int smanji(int k)
{
return --k;
}


int funpok(int* pp)
{
*pp = povecaj(*pp); /*UNSAFE, UNSAFE*/
*pp = povecaj(*pp); /*UNSAFE, UNSAFE*/
*pp = smanji(*pp);  /*UNSAFE, UNSAFE*/
*pp = smanji(*pp);  /*UNSAFE, UNSAFE*/
*pp = 3/(*pp);      /*UNSAFE, UNSAFE, UNSAFE*/
return *pp;         /*UNSAFE*/
}

int main()
{

int a = 5;
int b = 0;
int c = 5;
int* p;

c = funpok(&c); /*SAFE x12*/


p = &a;
*p = 5;  

c = funpok(p); /*SAFE x12 a = 0, c = 0*/
c = 3/a; /*FLAWED*/

p = &b;
*p = 3;
c = funpok(p); /*SAFE x12, c=1, b=1, *p=1*/


c = funpok(&c); /*SAFE x12*/

p = &b;
*p = 5; 
c = funpok(&b); /*SAFE x12*/


}


