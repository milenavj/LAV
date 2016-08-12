#include <stdio.h>

main() {

  float a11, a12, a21, a22, d;

  scanf("%f%f%f%f", &a11, &a12, &a21, &a22);

  d = a11 * a22 - a12 * a21;

  printf("%0.4f\n", d);

  return 0;
}
