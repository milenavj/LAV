
#include <stdio.h>

float check_main(float a11, float a12, float a21, float a22) {
  float r, m, n;
  // scanf("%f%f%f%f",&a11,&a12,&a21,&a22); /*ucitavamo brojeve*/
  r = a11 * a22; /*racunamo determinantu */
  m = a12 * a21;
  n = r - m;
  {
    printf("%.4f\n", n);
    return n;
  } /*stampamo */

}

#include "../check.h"
