#include <stdio.h>

	int main()
{

	int n,i,j;

	printf("Unesite n\n");
	scanf("%d",&n);

	if(n<=0) printf("-1");
	else
{

	for(j=1;j<=n+1;j++)
	{
		for(i=j;i<=n+1-j;i++)
		{
			printf("%d ",i);
		}
	}
}



printf("\n");







}
 
