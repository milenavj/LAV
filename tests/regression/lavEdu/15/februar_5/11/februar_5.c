#include <stdio.h>

int main()
{
	int n,i,j;
	scanf("%d",&n);
	if (n<=0) { printf("-1\n"); return 1; }
	for (i=0;i<n/2;i++)
	 {
		for (j=i+1;j<=n-i;j++)
			printf("%d ",j);
	 }
	if (n%2!=0) { printf("%d",n/2+1); }
	printf("\n");
	return 0;
}
