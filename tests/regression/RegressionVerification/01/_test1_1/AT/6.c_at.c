
#include <stdio.h>

int check_main(int e) {
  int jedinice, desetice, stotine, hiljade;

  printf("Unesite broj \n");
  // scanf ("%d", &e);
  if (e < 1000 || e > 9999) {
    printf("Rezultat je   : -1 \n");
    return -1;
  } else {
    jedinice = e % 10;
    desetice = (e / 10) % 10;
    stotine = (e / 100) % 10;
    hiljade = e / 1000;

    if (hiljade > stotine && stotine > desetice && desetice > jedinice) {
      printf("Rezultat je : 2 \n");
      return 2;
    } else if (hiljade < stotine && stotine < desetice && desetice < jedinice) {
      printf("Rezultat je : 1 \n");
      return 1;
    } else {
      printf("Rezultat je: 3 \n");
      return 3;
    }

  }
  // return 0;
}

#include "../check.h"
