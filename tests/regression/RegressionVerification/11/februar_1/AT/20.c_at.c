
#include <stdio.h>

float check_main(float a11, float a12, float a21, float a22) {
  float a;

  // scanf ("%f%f%f%f", &a11, &a12, &a21, &a22);

  a = (a11 * a22) - (a12 * a21);

  {
    printf("%7.4f \n", a);
    return a;
  }

  // return 0;
}

#include "../check.h"
