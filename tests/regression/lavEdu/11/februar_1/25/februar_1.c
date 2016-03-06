#include<stdio.h>
int main ()
{
  float a11, a12, a21, a22, M;
  printf("Unesi brojeve koji ce biti clanovi matrice:\n");
  scanf("%f%f%f%f", &a11, &a12, &a21, &a22);
  M=((a11*a22)-(a12*a21));
  printf("%1.4f\n", M);
  
return 0;  
}


