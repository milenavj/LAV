
#include <stdio.h>

int check_main(int x, int y, int z) {

  // scanf ("%d %d %d", &x, &y, &z);
  if (x > y && x > z) {
    printf("%d\n", x);
    return x;
  } else if (y > z) {
    printf("%d\n", y);
    return y;
  } else {
    printf("%d\n", z);
    return z;
  }
  // return 0;
}

#include "../check.h"
