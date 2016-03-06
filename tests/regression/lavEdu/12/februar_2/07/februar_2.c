#include<stdio.h>


int main()
{
  
  int a[10],i,max; //znam da je komplikovano, ali radi
 
  
  for(i=0;i<3;i++)
  {
  scanf("%d",&a[i]);
  }
  
  
  
  max=a[0];
  for(i=0;i<3;i++)
  {
   
   if(a[i]>max)
     max=a[i];
    
  }
  
  
  
  printf("Najveci je %d\n",max);
  
  
}

