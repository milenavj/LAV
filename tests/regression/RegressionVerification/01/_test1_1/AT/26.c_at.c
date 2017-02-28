
#include <stdio.h>
int check_main(int n) {
  int Cj, Cd, Cs, Ch;
  printf("Unesi cetvorocifren broj:\n");
  // scanf ("%d", &n);

  if (n > 999 && n < 10000) {
    Cj = n % 10;
    Cd = (n % 100) / 10;
    Cs = (n / 100) % 10;
    Ch = (n / 1000);

    if (Ch > Cs && Cs > Cd && Cd > Cj) {
      {
        printf("2\n");
        return 2;
      }
    } else if (Cj > Cd && Cd > Cs && Cs > Ch) {
      {
        printf("1\n");
        return 1;
      }
    } else {
      {
        printf("3\n");
        return 3;
      }
    }
  } else {
    {
      printf("-1\n");
      return -1;
    }
  }

  // return 0;
}

#include "../check.h"
