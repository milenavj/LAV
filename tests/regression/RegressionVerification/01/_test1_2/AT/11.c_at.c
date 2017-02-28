
#include <stdio.h>

int check_main(int x) {

  int a, b, c, d, proizvod, korekt;

  printf("Unesite pozitivan cetvorocifreni broj... ");
  // scanf("%d", &x);

  /*pocetna vrednost proizvoda */
  proizvod = 1;

  /*izdvajam cifre */
  a = x % 10;
  b = x % 100 / 10;
  c = x % 1000 / 100;
  d = x / 1000;

  /*proveravam da li je ispravan broj */
  if (x > 0 && (x >= 1000 && x <= 9999)) {

    if (a % 2 == 0)
      proizvod *= a;

    if (b % 2 == 0)
      proizvod *= b;

    if (c % 2 == 0)
      proizvod *= c;

    if (d % 2 == 0)
      proizvod *= d;

    korekt = 1;

  } else {
    printf(" -1 ");
    return -1;
  }

  if (korekt == 1)

    if (proizvod != 1) {
      printf("Proizvod je: %d ", proizvod);
      return proizvod;
    } else {
      printf(" 1  ");
      return 1;
    }

  // return 0;

}

#include "../check.h"
