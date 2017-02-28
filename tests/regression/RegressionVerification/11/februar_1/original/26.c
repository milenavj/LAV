#include <stdio.h>
int main() {
  float a11, a12, a21, a22, d;
  //  printf("Unesi matricu 2x2, redom a11,a12,a21,a22:\n");
  scanf("%f%f%f%f", &a11, &a12, &a21, &a22);
  d = a11 * a22 - a12 * a21;
  printf("determinanta je %.4f\n", d);
}