
#include <stdio.h>

int check_main(int a, int b, int c) {
  int max, max1;

  // scanf ("%d%d%d", &a, &b, &c);

  max = a > b ? a : b;
  max1 = max > c ? max : c;
  printf("%d\n", max1);
  return max1;
  // return 0;
}

#include "../check.h"
