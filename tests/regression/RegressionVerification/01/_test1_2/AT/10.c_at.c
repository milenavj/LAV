
#include <stdio.h>
int check_main(int x) {
  int m, i;
  printf("Upisite cetvorocifreni pozitivan broj:\n");
  // scanf("%d",&x);
  if (!(999 < x && x < 10000))
    i = 1;
  else {
    m = 1;
    i = 0;
    if ((x / 1000) % 2 == 0) {
      m *= (x / 1000);
      i++;
    }
    if (((x / 100) % 10) % 2 == 0) {
      m *= x / 100 % 10;
      i++;
    }
    if (((x / 10) % 10) % 2 == 0) {
      m *= x / 10 % 10;
      i++;
    }
    if ((x % 10) % 2 == 0) {
      m *= x % 10;
      i++;
    }
  }
  if (i == 0) {
    printf("1\n");
    return 1;
  } else if (i == 1) {
    printf("-1\n");
    return -1;
  } else {
    printf("Proizvod parnih cifara je:%d\n", m);
    return m;
  }
  // return 0;
}

#include "../check.h"
