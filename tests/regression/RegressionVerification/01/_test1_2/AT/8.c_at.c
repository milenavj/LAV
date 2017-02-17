
#include <stdio.h>
int check_main(int n) {
  int c, m, s = 1;
  printf("unesi jedan cetvorocifren pozitivan broj\n");
  // scanf("%d", &n);
  if (n < 0)
    m = -1;

  if (n >= 1000 && n <= 9999) {
    while (n > 0) {
      c = n % 10;
      if (c % 2 == 0)
        s *= c;
      n = n / 10;
    }
    {
      printf("rezultat je ; %d \n", s);
      return s;
    }
  } else
    m = -1;
  if (m == -1) {
    printf("%d/n", m);
    return m;
  }
  // return 0;
}

#include "../check.h"
