#include<stdlib.h>

int povecaj(int k)
{return ++k;}


int funpok(int* pp)
{
*pp = 3/(*pp); /*UNSAFE, UNSAFE, UNSAFE*/
return *pp;	/*UNSAFE*/
}

int main()
{

int a = 11;
int b = 0;
int c = 5;
int* p;

p = &a;
c = povecaj(*p); /*SAFE*/
c = c/(c-12); /*FLAWED*/


c = 5;
c = funpok(&c); /*SAFE, SAFE, SAFE, SAFE*/ 



p = &a;
*p = 5;  /*SAFE*/

c = funpok(p); /*SAFE, SAFE, SAFE, SAFE a = 0, c = 0*/
c = (b+3)/a; /*FLAWED*/

p = &b;
*p = 3; /*SAFE*/
c = funpok(p); /*SAFE, SAFE, SAFE, SAFE c=1, b=1, *p=1*/

c = funpok(&c); /*SAFE, SAFE, SAFE, SAFE */

p = &b;
*p = 5; /*SAFE*/
c = funpok(&b); /*SAFE, SAFE, SAFE, SAFE*/


}
