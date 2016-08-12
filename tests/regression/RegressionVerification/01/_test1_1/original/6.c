#include <stdio.h>

int main() {
  int jedinice, desetice, stotine, hiljade, e;

  printf("Unesite broj \n");
  scanf("%d", &e);
  if (e < 1000 || e > 9999)
    printf("Rezultat je   : -1 \n");
  else {
    jedinice = e % 10;
    desetice = (e / 10) % 10;
    stotine = (e / 100) % 10;
    hiljade = e / 1000;

    if (hiljade > stotine && stotine > desetice && desetice > jedinice)
      printf("Rezultat je : 2 \n");
    else if (hiljade < stotine && stotine < desetice && desetice < jedinice)
      printf("Rezultat je : 1 \n");
    else
      printf("Rezultat je: 3 \n");

  }
  return 0;
}
