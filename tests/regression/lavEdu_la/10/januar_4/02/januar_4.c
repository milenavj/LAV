  
#include<stdio.h>
#define max_size 100

int main()
{
	int n,i,j;
	char a[max_size][max_size];
	scanf("%d",&n);
	if(n%2==1 && n>0)
	{
		for(i=0;i<n;i++)
		{
			for(j=0;j<n;j++)
			{
			
				if(((i==0) || (i==n-1)) || (i==j) ||(i+j>=n-1 && (i>j))||(i+j<=n-1 && (i<j))) a[i][j]='x';
				else a[i][j]='.';
		
			}
		}

		for(i=0;i<n;i++)
		{
			for(j=0;j<n;j++)
			{
				printf("%c",a[i][j]);
			}	
			printf("\n");
		}
	}
	else printf("*\n");
	return 0;
	
}
