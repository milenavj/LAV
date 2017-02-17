
#include <stdio.h>

int check_main(int a) {

  // scanf ("%i", &a);
  //  printf ("%i \n", a);
  if (a < 0 || a > 9999 || a < 1000) {
    {
      printf("-1");
      return -1;
    }
    // return 0;
  }
  if (a / 1000 > (a % 1000) / 100 && (a % 1000) / 100 > (a % 100) / 10 &&
      (a % 100) / 10 > a % 10) {
    {
      printf("2");
      return 2;
    }
  } else if (a / 1000 < (a % 1000) / 100 && (a % 1000) / 100 < (a % 100) / 10 &&
             (a % 100) / 10 < a % 10) {
    {
      printf("1");
      return 1;
    }
  } else {
    {
      printf("3");
      return 3;
    }
  }
  printf("\n");
  // return 0;
}

#include "../check.h"
