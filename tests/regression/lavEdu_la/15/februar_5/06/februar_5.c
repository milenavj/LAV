#include<stdio.h>

int main()
{
	int n, i, j;
	scanf("%d", &n);
	if(n>0)
	{
		for(i=1; i<=n; i++)
		{
			for(j=i; j<=n; j++)
			{
				printf("%d ", j);
			}
			n--;
			
		}
	}
	else
		printf("-1\n");
	return 0;
}