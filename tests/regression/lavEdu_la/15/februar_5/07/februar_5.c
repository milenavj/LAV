#include<stdio.h>
int main(){
  int n,i=1,n1;
  printf("Unesi n:\n");
  scanf("%d",&n);
  n1=n;
  if(n>0){
    for(i;i<=n;i++){
      printf("%d ",i);
      if(i==n){
      n--;
      i=n1-n;
      }
    }
  }else printf("-1\n");
}