#include <stdio.h>

int main() {
  int a, b, c, d;

  printf("Unesite brojeve\n");
  scanf("%d%d%d", &a, &b, &c);

  d = (a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c);

  printf("Najveci je %d\n", d);

  return 0;
}
