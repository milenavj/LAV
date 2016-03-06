#include <stdio.h>
main()
{
  int n,i,b=1;
  printf("Unesi n: ");
  scanf("%d",&n);
 if(n>0)
 {
   
   while(n!=0)
  {
    for(i=b;i<=n;i++)
    {
      printf("%d ",i);
      
    }
    n--;
    b++;
  }
 }
   else  printf("-1\n");
}

