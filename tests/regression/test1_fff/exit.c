#include <malloc.h>
#include <stdlib.h>
void myexit()
{
exit(1);
}
int assert(int);
int proba(int s)
{
if (s == 0) exit(1);
assert(s!=0);		/*SAFE*/
return s;
}

int main()
{
int b;
if(b==0) myexit();
assert(b!=0);		/*SAFE*/

int a = proba(b);
assert(a!=0);		/*SAFE*/
int c;
a = proba(c);	
assert(a!=0);		/*SAFE*/
}
