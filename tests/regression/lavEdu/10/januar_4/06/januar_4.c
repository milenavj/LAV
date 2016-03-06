#include<stdio.h>

int main()
{
  int n,i,j,k,g;
  scanf("%d",&n); //unos broja n
  if(n<=0)	//provera da li je n paran
  {	printf("*\n");
    return 0;
  }
  
   if(n%2==0)	//provera da li je n paran
  {	printf("*\n");
    return 0;
  }
  
  for(i=0; i<(n-1)/2; i++)  //stampamo do pola
  { 
      for(j=0; j<i; j++)
        printf(".");  //stampamo i tackica
	  for(k=0; k<n-2*i; k++) 
	    printf("x"); // stampamo 2i-n
	       for(g=0; g<i; g++)
		    printf("."); 
  
	printf("\n");
     
  }
  
  for(i=1; i<(n+3)/2; i++)
  {   for(j=0; j<=n/2-i; j++)
	  printf(".");
	    for(k=0; k<2*i-1; k++)
		printf("x");
		  for(g=0; g<=n/2-i; g++)
		      printf(".");
    printf("\n");
  }
  return 0;
}