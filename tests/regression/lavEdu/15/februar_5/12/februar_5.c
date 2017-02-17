#include <stdio.h>

main () {
  int n,i,k;
  
  scanf("%d",&n);
  
  if(n < 1) {
    printf("-1\n");
    return 0;
  }
  
  for(k=0;k<((n/2)+1);k++)
  for(i=1+k;i<=n-k;i++)
  printf("%d ",i);
  
  
  printf("\n");
  
  return 0;
}
