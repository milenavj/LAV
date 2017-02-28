
#include <stdio.h>

float check_main(float a11, float a12, float a21, float a22) {
  float determinanta;

  // scanf("%f%f%f%f", &a11, &a12, &a21, &a22);
  determinanta = (a11 * a22) - (a12 * a21);
  {
    printf("%f", determinanta);
    return determinanta;
  }

  // return 0;
}

#include "../check.h"
