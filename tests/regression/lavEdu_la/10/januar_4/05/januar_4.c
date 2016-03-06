#include<stdio.h>

int main()
{
int n,i,j;
scanf("%d",&n);/*Unos velicine*/

if(n%2==0)/*Provera da li je paran broj*/
	{
	printf("*\n");
	return -1;
	}
for(i=0;i<n;i++)
{
for(j=0;j<n;j++)
	
	if(i<=n/2)
	if(j<i||j>=n-i)
		printf(".");
	else
		printf("x");
	else
		if(j>i||j<n-i-1)
			printf(".");
		else
			printf("x");
printf("\n");
}
return 0;
}


