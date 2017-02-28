
#include <stdio.h>

int check_main(int broj) {
  int jedinice, desetice, stotine, hiljade;
  printf("Unesite pozitivan cetvorocifren broj:\n");
  // scanf ("%d", &broj);
  if (broj < 1000 || broj > 9999) {
    {
      printf("\n-1\n\n\n");
      return -1;
    }
    // return 0;
  } else {
    jedinice = broj % 10;
    desetice = (broj / 10) % 10;
    stotine = (broj / 100) % 10;
    hiljade = broj / 1000;

    if (hiljade > stotine && stotine > desetice && desetice > jedinice) {
      printf("\n2\n");
      return 2;
    } else if (hiljade < stotine && stotine < desetice && desetice < jedinice) {
      printf("\n\n1\n");
      return 1;
    } else {
      printf("\n\n3\n");
      return 3;
    }

  }

}

#include "../check.h"
