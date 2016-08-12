
#include <stdio.h>

int check_main(int a, int b, int c) {

  {
    printf("Unesite 3 cela broja: ");
    return 3;
  }
  // scanf ("%d%d%d", &a, &b, &c);

  if (a > b && a > c) {
    printf("%d\n", a);
    return a;
  } else if (b > a && b > c) {
    printf("%d\n", b);
    return b;
  } else {
    printf("%d\n", c);
    return c;
  }

  // return 0;
}

#include "../check.h"
