
#include <malloc.h>
#include <stdlib.h>
int assert(int); int assume(int);
int* mymalloc(int size)
{
int* p = malloc(size);
if(p == 0) exit(1);
return p;
}

int* mmmalloc(int size)
{
return mymalloc(size);
}

int main()
{

int* a = mmmalloc(100);
int* b = mymalloc(100);


a[0] = 3;
b[0] = 4;
assert(a!=b);

assert(a!=0);
assert(b!=0);
assert(a[0] == 3);


int c = 3;
a = malloc(100);
b = malloc(100);
assume(a!=0);
assume(b!=0);

if(c>0) c++;
else c--;

a[c] = 3;
b[c] = 4;
assert(a[c] == 3);

a = mymalloc(100);
b = mymalloc(100);

a[c+1] = 3;
b[c+1] = 4;

assert(a[c+1] == 3);

}
