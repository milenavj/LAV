
#include <stdio.h>

float check_main(float a11, float a12, float a21, float a22) {

  float a;

  printf("Unesite vrednosti\n");
  // scanf("%f%f%f%f",&a11, &a12, &a21, &a22);

  a = a11 * a22 - a21 * a12;

  {
    printf("Rezultat je  %.4f\n", a);
    return a;
  }

  // return 0;
}

#include "../check.h"
