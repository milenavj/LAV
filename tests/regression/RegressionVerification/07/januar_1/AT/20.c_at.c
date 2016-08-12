
#include <stdio.h>

int check_main(float a, float b) {
  /*ispod se unose 2 broja tipa float, porede se i na osnovu rezultata
   * poredjenja stampa se izlaz */

  printf("Unesi prvi broj\n");
  // scanf("%f", &a);

  printf("Unesi drugi broj\n");
  // scanf("%f", &b);

  if (a > b) {
    printf("1\n");
    return 1;
  } else {
    printf("2\n");
    return 2;
  }

  // return 0;
}

#include "../check.h"
