#include <stdio.h>



int main()
{
	int n,i,g=0,k;
	scanf("%d", &n);
	if(n%2==0)
	printf("*\n");
	else
{
	for(k=0;k<n/2+1;k++)
	{
	for(i=0;i<g;i++)
	printf(".");
	for(i=0;i<n-2*g;i++)
	printf("x");
	for(i=0;i<g;i++)
	printf(".");
	printf("\n");
	g++;
	}
	g=g-2;
		for(k=0;k<n/2;k++)
	
	{
	for(i=0;i<g;i++)
	printf(".");
	for(i=0;i<n-2*g;i++)
	printf("x");
	for(i=0;i<g;i++)
	printf(".");
	printf("\n");
	g--;
	
}
}

return 0;
}
