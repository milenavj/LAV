
#include <stdio.h>

int check_main(int a, int b, int c) {
  int d;

  printf("Unesite brojeve\n");
  // scanf ("%d%d%d", &a, &b, &c);

  d = (a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c);

  {
    printf("Najveci je %d\n", d);
    return d;
  }

  // return 0;
}

#include "../check.h"
