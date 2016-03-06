#include<stdio.h>

int main()

{
  float a, b;/*ispod se unose 2 broja tipa float, porede se i na osnovu rezultata poredjenja stampa se izlaz*/
  
  printf("Unesi prvi broj\n");
  scanf("%f", &a);
  
  printf("Unesi drugi broj\n");
  scanf("%f", &b);
  
  if(a>b) 
    printf("1\n");
  else 
    printf("2\n");

return 0;
}
