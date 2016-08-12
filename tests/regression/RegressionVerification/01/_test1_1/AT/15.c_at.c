
#include <stdio.h>
int check_main(int broj) {
  int cj, cd, cs, ch, rez; //deklarisanje promenljivih
  printf("\nUnesi jedan pozitivan cetvorocifren broj: ");
  // scanf ("%d", &broj);               //unos broja
  if ((broj > 999) &&
      (broj < 10000)) //provera broja da li nije cetvorocifren i pozitivan
      {
    cj = broj % 10;           //izdvajam cifru jedinica
    cd = (broj % 100) / 10;   //izdvajam cifru desetica
    cs = (broj % 1000) / 100; //izdvajam cifru stotina
    ch = broj / 1000;         //i cifra hiljada
    if ((ch < cs) && (cs < cd) && (cd < cj))
      rez = 1; //rastuce cifre
    else if ((ch > cs) && (cs > cd) && (cd > cj))
      rez = 2; //opadajuce cifre
    else
      rez = 3; //unos je korektan ali nije ni rastuce ni opadajuce
  } else {
    rez = -1; //unos nije korektan
  }
  {
    printf("\nRezultat je: %d \n", rez);
    return rez;
  } //stampa
    // return 0;
}

#include "../check.h"
