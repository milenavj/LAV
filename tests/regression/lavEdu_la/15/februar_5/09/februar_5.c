#include<stdio.h>
   int main()
     {
        int n,i,k;
        scanf("%d",&n);
        k=1;
if(n<1) printf("-1");
        while(k<=n)
           {
         
             for(i=k;i<=n;i++)
                   printf("%d ",i);
             k++;
             n--;
         }
   return 0;
 }
               
        
        
       