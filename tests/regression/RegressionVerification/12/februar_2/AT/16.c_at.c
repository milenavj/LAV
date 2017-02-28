
#include <stdio.h>

int check_main(int a, int b, int c) {

  int pom;
  // scanf ("%d %d %d", &a, &b, &c);
  if (a > b && a > c)
    pom = a;
  else if (b > c)
    pom = b;
  else
    pom = c;

  {
    printf("%d \n", pom);
    return pom;
  }

  // return 0;
}

#include "../check.h"
