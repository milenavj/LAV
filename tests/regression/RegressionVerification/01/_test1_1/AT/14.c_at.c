
#include <stdio.h>

int check_main(int x) {
  int h, s, d, j;
  /*
     h - cifra hiljade
     s - cifra stotine
     d - cifra desetice
     j - cifra jedinice
     x - uneti broj
   */

  printf("Unesite pozitivan cetvorocifreni broj: \n");
  // scanf ("%d", &x);

  if (x < 1000 || x > 9999) {
    printf("-1\n");
    return -1;
  } else {
    h = x / 1000;
    s = (x / 100) % 10;
    d = (x / 10) % 10;
    j = x % 10;

    if (j > d && d > s && s > h) {
      printf("Rezultat je: 1\n");
      return 1;
    } else if (h > s && s > d && d > j) {
      printf("Rezultat je: 2\n");
      return 2;
    } else {
      printf("Rezultat je: 3\n");
      return 3;
    }

  }
  // return 0;
}

#include "../check.h"
