
#include <stdio.h>

int check_main(int a, int b, int c) {
  int max;
  // scanf ("%d%d%d", &a, &b, &c);
  max = a;
  if (b > max)
    max = b;
  if (c > max)
    max = c;

  {
    printf("%d\n", max);
    return max;
  }

  // return 0;
}

#include "../check.h"
