#include <stdio.h>
int main() {
  float a, b;
  printf("Uneisite a i b\n");
  scanf("%f%f", &a, &b);
  if (a > b)
    printf("1");
  else
    printf("2"); //Poredjenje brojeva
  return 0;
}