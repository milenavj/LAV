#include <stdio.h>


int main()
{
 int i,j,n,a=0,b,p=0,k;
 
 printf("Unesi n:\n");
 scanf("%d",&n);
 
 if (((n%2)==0)||(n<=0) )
  printf("*\n");
 else
  {
   b=n;
   for (i=0; i<n; i++)
    {
     for (j=0; j<a; j++)
      printf(".");
     for (k=0; k<b; k++)
      printf("x");
     for (j=0; j<a; j++)
      printf(".");
     
     printf("\n");
     if (b==1) 
      p=1;
     if (p==1)
      {
       b=b+2;
       a=a-1;
      }
     else
      {     
       b=b-2;
       a=a+1;
      } 
    }
   }
  return 0;
}

     
