#include <stdio.h>

main() {
  float a11, a12, a21, a22;
  float D;

  scanf("%f%f%f%f", &a11, &a12, &a21, &a22);

  D = a11 * a22 - a12 * a21;

  printf("%.4f/n", D);
  return 0;
}
