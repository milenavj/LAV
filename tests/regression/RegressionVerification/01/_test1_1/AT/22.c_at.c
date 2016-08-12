
#include <stdio.h>

int check_main(int e) {
  int a, b, c, d;
  printf("Unesite pozitivan cetvorocifren broj: ");
  // scanf ("%i", &e);
  if (e >= 1000 && e < 10000) {
    d = e % 10;
    c = ((e - d) % 100) / 10;
    b = ((e - d - 10 * c) % 1000) / 100;
    a = ((e - d - 10 * c - 100 * b) % 10000) / 1000;

    if (a > b && b > c && c > d) {
      printf("2");
      return 2;
    } else if (a < b && b < c && c < d) {
      printf("1");
      return 1;
    } else {
      printf("3");
      return 3;
    }
  } else {
    printf("-1");
    return -1;
  }
  // return 0;
}

#include "../check.h"
