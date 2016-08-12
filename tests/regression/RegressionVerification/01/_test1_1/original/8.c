#include <stdio.h>

int main() {

  unsigned int broj;
  int rastuce, opadajuce, nisu_uredjene, desetice, jedinice, stotine, hiljade;

  printf("Unesite pozitivan cetvorocifreni broj: ");
  scanf("%d", &broj);

  if (broj < 1000 || broj > 9999) {
    printf("-1");
    return 1;
  }

  jedinice = broj % 10;
  desetice = (broj / 10) % 10;
  stotine = (broj / 100) % 10;
  hiljade = broj / 1000;

  if (jedinice > desetice && desetice > stotine && stotine > hiljade) {
    printf("1");

  } else if (hiljade > stotine && stotine > desetice && desetice > jedinice) {
    printf("2");

  } else
    printf("-1");
  return 0;

}
