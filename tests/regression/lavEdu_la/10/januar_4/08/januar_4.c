#include<stdio.h>


int main() 
{
	int n,i=0,j=0,k=0;
	scanf("%d",&n);
	if(n%2==0)
		printf("*\n");
	else
	{
		for (i=0;i<n;i++)
			printf("x");
		printf("\n");
		for (i=1;i<n;i++)
		{
			if (i<n/2+1)
			{
				if(i>0)
					for (j=0;j<i;j++)
					printf(".");
				for (j=0;j<n-2*i;j++)
					printf("x");
				if (i>0)
					for (j=0;j<i;j++)
					printf(".");
				printf("\n");
			}
			else
			{
				if(i>0)
					for (j=0;j<n-i-1;j++)
					printf(".");
				for (k=0;k<n-2*j;k++)
					printf("x");
				if (i>0)
					for (j=0;j<n-i-1;j++)
					printf(".");
				printf("\n");	
			}	
		} printf("\n");
	}
	return 0;
}
