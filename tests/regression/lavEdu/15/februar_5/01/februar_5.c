#include<stdio.h>
main(){
  int n,k=1,i;
  printf("Unesite broj n:");
  scanf("%d",&n);
  if(n<=0){printf("-1\n");return 1;}
  for(i=k;i<=n;i++){
     printf("%d ",i);
     if(i==n){
     i=k++;
     n=n--;
     }
  }
  printf("\n");
}
