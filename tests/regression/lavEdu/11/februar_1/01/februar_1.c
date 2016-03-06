#include<stdio.h>
int main(){
      float a11,a12,a21,a22;
//      printf("Unesite a11, a12, a21 i a22:\n");
      scanf("%f%f%f%f",&a11,&a12,&a21,&a22);
      printf("Izlaz:\n%.4f\n",a11*a22-a12*a21);
      return 0;
      }

