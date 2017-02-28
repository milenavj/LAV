
#include <stdio.h>

int check_main(int x) {
  int j, d, s, h;
  printf("Unesite pozitivan cetvorocifreni broj.\n");
  // scanf ("%d", &x);
  if (x < 1000 || x > 9999 || x < 0) {
    printf("-1\n");
    return -1;
  } else {
    j = x % 10;
    d = x % 100 / 10;
    s = x % 1000 / 100;
    h = x / 1000;
    if ((j < d && d < s && s < h) || (j > d && d > s && s > h)) {
      if (j < d && d < s && s < h) {
        printf("2 \n");
        return 2;
      } else {
        printf("1 \n");
        return 1;
      }
    } else {
      printf("3 \n");
      return 3;
    }
  }

  // return 0;
}

#include "../check.h"
