#include<stdio.h>
int main ()
{
 float a11,a12,a21,a22;
 float det;
 
 scanf("%f%f%f%f",&a11,&a12,&a21,&a22);
 det= a11*a22 - a12*a21;
 printf("%.4f",det);
 

return 0;
} 
