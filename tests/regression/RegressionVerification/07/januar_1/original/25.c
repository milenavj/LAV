#include <stdio.h>

int main() {
  float a, b;
  scanf("%f%f", &a, &b);
  if (a > b)
    printf("%d\n", 1);
  else
    printf("%d\n", 2);

  return 0;
}
