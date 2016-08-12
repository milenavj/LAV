
#include <stdio.h>

int check_main(int a, int b, int c) {

  // scanf ("%d", &a);
  // scanf ("%d", &b);
  // scanf ("%d", &c);
  if (a > b) {
    if (a > c) {
      printf("%d", a);
      return a;
    } else if (c > b) {
      printf("%d", c);
      return c;
    }
  } else if (c > a) {
    if (c > b) {
      printf("%d", c);
      return c;
    } else if (b > c) {
      printf("%d", b);
      return b;
    }
  } else {
    printf("%d\n", b);
    return b;
  }

  // return 0;

}

#include "../check.h"
