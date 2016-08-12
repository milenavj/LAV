
#include <stdio.h>
int check_main(int broj) {
  int cj, cs, cd, ch, rez;
  printf("Unesite cetvorocifreni broj");
  // scanf ("%d", &broj);
  if ((broj < 1000) || (broj > 9999))
    rez = -1;
  else {
    cj = broj % 10;
    cd = (broj / 10) % 10;
    cs = (broj / 100) % 10;
    ch = (broj / 1000) % 10;
    if ((cj > cd) && (cd > cs) && (cs > ch))
      rez = 1;
    else if ((cj < cd) && (cd < cs) && (cs < ch))
      rez = 2;
    else
      rez = 3;
  }
  {
    printf("%d", rez);
    return rez;
  }
  // return 0;
}

#include "../check.h"
