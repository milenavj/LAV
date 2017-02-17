#include <stdio.h>

int main() {
  float a11, a12, a21, a22, a;

  scanf("%f%f%f%f", &a11, &a12, &a21, &a22);

  a = (a11 * a22) - (a12 * a21);

  printf("%7.4f \n", a);

  return 0;
}
