#include<stdio.h>


int main()
{
float a11, a12, a21, a22, r;

printf("Unesi brojeve:\n");
scanf("%f%f%f%f", &a11, &a12, &a21, &a22);

r= a11*a22 - a12*a21;

printf("Resenje determinante je: %f ", r);



return 0;
}



