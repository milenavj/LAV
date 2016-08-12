#include <stdio.h>
int main() {
  float a11, a12, a21, a22, D;

  scanf("%f %f %f %f", &a11, &a12, &a21, &a22);

  D = a11 * a22 - a12 * a21;

  printf("Izlaz: %f \n", D);

  return 0;
}
