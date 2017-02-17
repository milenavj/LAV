#include<stdio.h>

int main()
{
int n,i,k;
scanf("%d",&n);
if(n<=0){
 printf("-1\n");
 return -1;
}
k=1;
while(k<=n){
 for(i=k;i<=n;i++)
   printf("%d ",i);
 n--;
 k++;  
}
printf("\n");
return 0;
}
