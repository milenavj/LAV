
#include <stdio.h>

float check_main(float a11, float a12, float a21, float a22) {

  float d;

  // scanf("%f%f%f%f",&a11,&a12,&a21,&a22);

  d = a11 * a22 - a12 * a21;

  {
    printf("%0.4f\n", d);
    return d;
  }

  // return 0;
}

#include "../check.h"
