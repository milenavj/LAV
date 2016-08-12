#include <stdio.h>

int main() {
  float a;
  float b;

  scanf("%f", &a);
  scanf("%f", &b);

  if (a > b)
    printf("1\n");
  else
    printf("2\n");

  return 0;
}
