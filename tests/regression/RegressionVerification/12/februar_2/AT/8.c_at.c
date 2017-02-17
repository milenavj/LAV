
#include <stdio.h>

int check_main(int a, int b, int c) {
  int max;

  //printf("Unesite 3 broja:\n");
  // scanf ("%d %d %d", &a, &b, &c);

  max = a;

  if (max < b)
    max = b;
  if (max < c)
    max = c;

  {
    printf("Najveci od unetih brojeva je %d\n", max);
    return max;
  }

  /*Mislim da za prva dva zadatka nisu potrebni komentari*/
  // return 0;
}

#include "../check.h"
