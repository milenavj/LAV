

#include <stdio.h>

int check_main(int n) {
  int cif1, cif2, cif3, cif4;
  printf("Unesite pozitivan cetvorocifren broj.\n");
  // scanf ("%d", &n);
  cif1 = n / 1000;
  cif2 = n / 100 % 10;
  cif3 = n / 10 % 10;
  cif4 = n % 10;

  if (n < 1000 || n > 9999) {
    printf("Rezultat je: -1.\n");
    return -1.;
  } else if (cif1 < cif2 && cif2 < cif3 && cif3 < cif4) {
    printf("Rezultat je: 1.\n");
    return 1.;
  } else if (cif1 > cif2 && cif2 > cif3 && cif3 > cif4) {
    printf("Rezultat je: 2.\n");
    return 2.;
  } else {
    printf("Rezultat je: 3.\n");
    return 3.;
  }
}

#include "../check.h"
