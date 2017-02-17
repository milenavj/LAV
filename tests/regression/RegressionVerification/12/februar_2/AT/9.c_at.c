
#include <stdio.h>
int check_main(int a, int b, int c) {

  int max;
  // scanf ("%d%d%d", &a, &b, &c);
  max = a;
  if (max < b)
    max = b;
  if (max < c)
    max = c;
  {
    printf("%d", max);
    return max;
  }
  // return 0;
}

#include "../check.h"
