
#include <stdio.h>
float check_main(float a, float b, float c, float d) {

  float z = 0;
  printf("Unesite brojeve");
  // scanf("%f", &a);
  // scanf("%f", &b);
  // scanf("%f", &c);
  // scanf("%f", &d);
  z = ((a * d) - (b * c));
  {
    printf("%.4f", z);
    return z;
  }
  // return 0;
}

#include "../check.h"
