
#include <stdio.h>
int check_main(int n) {
  int jed, des, stot, hilj;
  printf("Uneti pozitivan cetvorocifren broj:\n");
  // scanf ("%d", &n);
  if (n < 1000 || n > 9999) {
    {
      printf("-1\n");
      return -1;
    }
    // return 0;

  }
  jed = n % 10;
  des = (n / 10) % 10;
  stot = (n / 100) % 10;
  hilj = (n / 1000) % 10;
  if (jed < des && des < stot && stot < hilj) {
    {
      printf("2\n");
      return 2;
    }
    // return 0;
  }
  if (jed > des && des > stot && stot > hilj) {
    {
      printf("1\n");
      return 1;
    }
    // return 0;
  } else {
    printf("3\n");
    return 3;
  }
  // return 0;
}

#include "../check.h"
