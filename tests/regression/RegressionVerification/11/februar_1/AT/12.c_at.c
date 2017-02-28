
#include <stdio.h>

float check_main(float a11, float a12, float a21, float a22) {

  float resenje;

  printf("Unesite matricu: ");
  // scanf ("%f%f%f%f", &a11, &a12, &a21, &a22);

  resenje = (a11 * a22) - (a12 * a21);

  {
    printf("%4f\n", resenje);
    return resenje;
  }

  // return 0;
}

#include "../check.h"
