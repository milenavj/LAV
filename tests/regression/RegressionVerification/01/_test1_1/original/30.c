#include <stdio.h>

int main() {
  int j, d, s, h, broj;

  printf("\nunesite jedan pozitivan cetvorocifren broj\n");
  scanf("%d", &broj);

  if (broj < 0) {
    printf("-1\n");
    return 1;
  }
  if ((broj > 0 && broj < 1000) || broj > 9999) {
    printf("-1\n");
    return 1;
  }

  j = broj % 10;
  d = (broj / 10) % 10;
  s = (broj / 100) % 10;
  h = broj / 1000;
  broj = h + 100 * s + 10 * d + j;

  if (j > d && d > s && s > h) {
    printf("1\n");
    return 1;
  }
  if (h > s && s > d && d > j) {
    printf("2\n");
    return 1;
  }
  if (broj == h + 100 * s + 10 * d + j) {
    printf("3\n");
    return 1;
  }

  return 0;

}
