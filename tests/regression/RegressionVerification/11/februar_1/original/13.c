#include <stdio.h>

int main() {
  float determinanta, a11, a12, a21, a22;

  // printf("Unesite elemente matrice 2x2\n");
  scanf("%f %f %f %f", &a11, &a12, &a21, &a22);

  determinanta = a11 * a22 - a12 * a21;

  printf("%.4f\n", determinanta);

  return 0;
}
