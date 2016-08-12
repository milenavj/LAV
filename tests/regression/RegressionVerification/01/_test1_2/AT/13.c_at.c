
#include <stdio.h>
int check_main(int broj) {
  int k, x = 1;
  printf("Unesite pozitivan cetvorocifreni broj\n");
  // scanf ("%d", &broj);
  if (broj >= 1000 && broj <= 9999) {
    while (broj != 0) {
      k = broj % 10;
      if (k % 2 == 0)
        x = x * k;
      broj = broj / 10;
    }
    {
      printf("%d\n", x);
      return x;
    }
  } else if (broj <= 999 || broj >= 10000) {
    printf("-1\n");
    return -1;
  }

  // return 0;
}

#include "../check.h"
