
#include <stdio.h>

float check_main(float a11, float a12, float a21, float a22) {
  int i, j;
  float d;
  // scanf("%f",&a11);
  // scanf("%f",&a12);
  // scanf("%f",&a21);
  // scanf("%f",&a22);
  d = a11 * a22 - a12 * a21;
  {
    printf("%f\n", d);
    return d;
  }

  // return 0;
}

#include "../check.h"
