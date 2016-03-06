#include<stdio.h>

int main ()
{
	int n, i, j;
	scanf("%d", &n);
	if(n<=0 || n%2==0)
		{printf("*\n");
		return 0;}
	for(i=0;i<n;i++)
		{for(j=0;j<n;j++)
		{	if(i<=j && i+j<n || n-i-1<=j && n-i+j-1<n)
				printf("x");
			else
				printf(".");
		}
		printf("\n");}
return 0;
}



