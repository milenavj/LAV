
#include <stdio.h>
float check_main(float a11, float a12, float a21, float a22) {
  float D;
  printf("Uneti clanove matrice: ");
  // scanf("%f%f%f%f",&a11,&a12,&a21,&a22);
  D = (a11 * a22) - (a12 * a21);
  {
    printf("D=%.4f\n", D);
    return D;
  }
}

#include "../check.h"
