/* malloc.c */
#include<stdlib.h>

main()
{
int* p;
int* q;
p = malloc(sizeof(int)*10);
if(p==0) exit(0);
p[3] = 7; /*safe*/
q = p;
q[p[3]] = 5; /*safe, safe*/
p[10] = 7; /*flawed*/
}


