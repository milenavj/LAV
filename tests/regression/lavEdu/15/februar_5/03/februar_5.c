#include<stdio.h>
int main ()
{
  int i,n,j;
  scanf("%d",&n);
  if(n<=0) 
    { 
       printf("-1");
       return 1;
    }

  for(i=1;i<=n;i++)
    printf("%d ",i);
 for(j=0;j<=n;j++)
 for(i=2+j;i<n-j;i++)
    printf("%d ",i);
return 0;
}
