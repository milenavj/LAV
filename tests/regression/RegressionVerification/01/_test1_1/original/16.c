#include <stdio.h>
int main() {
  int broj, j, d;
  int s, h;
  int rezultat = -1;
  int rezultat1 = 1;
  int rezultat2 = 2;
  int rezultat3 = 3;
  printf("Unesi jedan pozitivan cetvorocifren broj: ");
  scanf("%d", &broj);
  if (broj < 10) /*ako je broj jednocifren*/
      {
    printf("Rezultat je: %d\n", rezultat);
  } else if (broj < 100) /*ako  je broj dvocifren*/
      {
    printf("Rezultat je: %d\n", rezultat);
  } else if (broj <= 999) /*ako je broj trocifren*/
      {
    printf("Rezultat je: %d\n", rezultat);
  } else if (broj > 9999) /*ako je broj petocifren*/
      {
    printf("Rezultat je: %d\n", rezultat);
  } else if (broj < 0) /*ako je broj negativan*/
      {
    printf("Rezultat je: %d\n", rezultat);
  } else {
    j = broj % 10;               /*racunamo cifru jedinica*/
    d = (broj / 10) % 10;        /*racunamo cifru desetica*/
    s = (broj / 100) % 10;       /*racunamo cifru stotina*/
    h = broj / 1000;             /*racunamo cifru hiljada*/
    if (j > d && d > s && s > h) /*ako su cifre poredjane u rastucem poretku*/
        {
      printf("Rezultat je: %d\n", rezultat1);
    } else if (h > s && s > d &&
               d > j) /*ako su cifre poredjane u opadajucem poretku*/
        {
      printf("Rezultat je: %d\n", rezultat2);
    } else
      printf("Rezultat je: %d \n", rezultat3); /*ako cifre nisu uredjene*/
  }
  return 0;
}
