#include <stdio.h>

int main() {
  int i, j;
  float a11, a21, a22, a12, d;
  scanf("%f", &a11);
  scanf("%f", &a12);
  scanf("%f", &a21);
  scanf("%f", &a22);
  d = a11 * a22 - a12 * a21;
  printf("%f\n", d);

  return 0;
}
