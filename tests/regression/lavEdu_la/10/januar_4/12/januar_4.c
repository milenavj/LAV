#include<stdio.h>
int main()
{
  int n,i,j,m=0,s;
  printf("Unesite neparan broj\n");		   //Unos dimenzije kvadrata		
  scanf("%d",&n);
  s=n; 
  if((n%2)==0) {
		printf("*\n");
		return 0;
	       }
    while(n>1)
     {
      for(i=0;i<m;i++) printf(".");
      for(i=0;i<n;i++) printf("x");
      for(i=0;i<m;i++) printf(".");
      n=n-2;
      m=m+1;
      printf("\n");	  
     }
    while(n<=s)                                     //Stampa kvadrata
     {
      for(i=0;i<m;i++) printf(".");
      for(i=0;i<n;i++) printf("x");
      for(i=0;i<m;i++) printf(".");
      n=n+2;
      m=m-1;
      printf("\n");	  
     }   
   return 0;
}