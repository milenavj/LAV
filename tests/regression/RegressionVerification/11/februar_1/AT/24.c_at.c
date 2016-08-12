
#include <stdio.h>

float check_main(float a11, float a12, float a21, float a22) {
  float r;

  printf("Unesi brojeve:\n");
  // scanf("%f%f%f%f", &a11, &a12, &a21, &a22);

  r = a11 * a22 - a12 * a21;

  {
    printf("Resenje determinante je: %f ", r);
    return r;
  }

  // return 0;
}

#include "../check.h"
