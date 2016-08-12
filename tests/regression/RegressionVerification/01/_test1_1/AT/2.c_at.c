
#include <stdio.h>

int check_main(int N) {
  int J, D, S, H, rezultat; /*promenljive N - prirodan broj, J - cifra jedinica
S - cifra stotina D - cifra desetica H - cifra hiljada */
  printf("Unesi prirodan cetvorocifreni broj.\n");
  // scanf ("%d", &N);
  if ((N > 9999) || (N < 1000))
    rezultat = -1; //provera korektnosti unosa
  else {
    J = N % 10;         //dobijamo cifru jedinica
    D = (N / 10) % 10;  //dobijamo cifru desetica
    S = (N / 100) % 10; //dobijamo cifru stotina
    H = N / 1000;       //dobijamo cifru hiljada
    if ((H > S) && (S > D) && (D > J))
      rezultat = 2; /*da li je poredak opadajuci */
    else if ((J > D) && (D > S) && (S > H))
      rezultat = 1; /*da li je rastuci */
    else
      rezultat = 3;
  }
  {
    printf("Rezultat je: %d\n", rezultat);
    return rezultat;
  } //ispis rezultata
}

#include "../check.h"
