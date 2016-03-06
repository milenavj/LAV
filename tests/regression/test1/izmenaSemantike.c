#include<stdio.h>
int main()
{
int a;

scanf("%d", &a);

while(a!=0)
{
a = 3/a; /*SAFE*/
scanf("%d", &a);
}

a = 3/a; /*FLAWED*/
printf("%d\n", a);
return 0;
}

/*

main()
{
int a;

scanf("%d", &a);
a = 3/a;

if(a!=0)
do
{
scanf("%d", &a);
a = 3/a;
}
while(a!=0)

printf("%d\n", a);
}

*/