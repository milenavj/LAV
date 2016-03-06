#include<stdio.h>
#define MAX_SIZE 21


int strcspn(char s[],char t[])
{
int i,j,br;
for(i=0;s[i];i++)
{
	for(j=0;t[j];j++)
		{
		if(s[i]==t[j])
			{
			br=i;
			return br;
			}
		}
}
return -1;
}

int main()
{
char s[MAX_SIZE];
char t[MAX_SIZE];
int i=0,j=0,br;
printf("Unesi s: ");
scanf("%s",&s);
printf("Unesi t: ");
scanf("%s",&t);

br=strcspn(s,t);
printf("%d\n",br);

return 0;
}