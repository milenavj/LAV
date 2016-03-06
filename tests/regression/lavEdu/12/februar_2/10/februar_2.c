#include<stdio.h>
int main(){
  int a,b,c,max;
//  printf("Unesi 3 broj:\n");
  scanf("%d%d%d",&a,&b,&c);
  max=a;
  if(b>max)max=b;
  if(c>max)max=c;
  printf("max = %d\n",max);
}

