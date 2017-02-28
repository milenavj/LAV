
#include <stdio.h>
#include <math.h>

int check_main(int n) {
  int h, s, d, j;

  printf("Unesi jedan pozitivan cetvorocifren broj\n");
  // scanf ("%d", &n);
  if (n > 9999 || n < 1000) {
    printf("-1\n");
    return -1;
  } else {
    h = n / 1000;
    s = (n % 1000) / 100;
    d = ((n % 1000) % 100) / 10;
    j = ((n % 1000) % 100) % 10;

    if (j > d && d > s && s > h) {
      printf("1\n");
      return 1;
    } else {
      if (h > s && s > d && d > j) {
        printf("2\n");
        return 2;
      } else {
        printf("3\n");
        return 3;
      }
    }
  }
  // return 0;
}

#include "../check.h"
