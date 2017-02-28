
#include <stdio.h>
int check_main(int a, int b, int c) {
  int max;
  // scanf ("%d", &a);
  // scanf ("%d", &b);
  // scanf ("%d", &c);
  max = a;
  if (b > max && b > c)
    max = b;
  else if (c > max)
    max = c;
  {
    printf("%d\n", max);
    return max;
  }
  // return 0;
}

#include "../check.h"
