
#include <stdio.h>
#include <math.h>

int check_main(int a) {
  int cif1, cif2, cif3, cif4;
  printf("Unesite pozitivan cetvorocifren broj");
      /*Unosimo cetvorocifren broj */
  // scanf ("%d", &a);
  if (a > 999 && a < 10000 && a > 0) { /*Ispitujemo dali je dobar unos */
    cif1 = a % 10;                     /*Dolazimo do poslednje cifre */
    cif2 = (a % 100) / 10;             /*Dolazimo do preposlednje cifre */
    cif3 = (a % 1000) / 100;           /*Dolazimo do druge cifre */
    cif4 = a / 1000;                   /*Dolazimo do prve cifre */
    if (cif4 < cif3 && cif3 < cif2 &&
        cif2 < cif1) { /*Ispitujemo dali su cifre poredjane po rastucem redu */
      printf("Rezultat je: 1 \n");
      return 1;
    } else if (cif4 > cif3 && cif3 > cif2 && cif2 > cif1) {
                 /*ispitujemo dali su cifre poredjane po opadajucem redu */
      printf("Rezultat je: 2 \n");
      return 2;
    } else {
      printf("Rezultat je: 3 \n");
      return 3;
    } /*Ispisujemo rezultat da cifre nisu poredjane po nijednom od navedenih
redosleda */
  } else {
    printf("-1 \n");
    return -1;
  }
  // return 0;
}

#include "../check.h"
