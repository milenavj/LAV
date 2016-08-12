#include <stdio.h>
int main() {
  float a, b;
  printf("Unesite prvi broj: ");
  scanf("%f", &a);
  printf("Unesite drugi broj: ");
  scanf("%f", &b);
  if (a > b) {
    printf("1\n");
    return 0;
  } else {
    printf("2\n");
    return 0;
  }
}
