
#include <stdio.h>
int check_main(int a, int b, int c) {

  printf("Unesite tri cela broja:\n");
  // scanf ("%d%d%d", &a, &b, &c);

  if (a > b) {
    {
      printf("Najveci broj je: %d", (a > c ? a : c));
      return (a > c ? a : c);
    }
  } else {
    {
      printf("Najveci je %d\n", (b > c ? b : c));
      return (b > c ? b : c);
    }
  }
  // return 0;
}

#include "../check.h"
