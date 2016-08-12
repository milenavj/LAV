#include <stdio.h>

int main() {
  int a, b, c, max;

  //printf("Unesite 3 broja:\n");
  scanf("%d %d %d", &a, &b, &c);

  max = a;

  if (max < b)
    max = b;
  if (max < c)
    max = c;

  printf("Najveci od unetih brojeva je %d\n", max);

  /*Mislim da za prva dva zadatka nisu potrebni komentari*/
  return 0;
}
