
#include <stdio.h>

float check_main(float a11, float a12, float a21, float a22) {

  // scanf("%f%f%f%f",&a11,&a12,&a21,&a22);
  {
    printf("%.4f\n", a11 * a22 - a12 * a21);
    return a11 * a22 - a12 * a21;
  }

  // return 0;
}

#include "../check.h"
