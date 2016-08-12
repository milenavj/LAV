
#include <stdio.h>
#include <math.h>
int check_main(int broj) {
  int cifra0, cifra1, cifra2, cifra3, proizvod;
  printf("Unesite prirodan cetvorocifren broj\n");
  // scanf("%i", &broj);
  proizvod = 1; //proizvod cifara pre programa postavlja se na 1
  if (broj < 1000 ||
      broj > 9999) //postavlja se uslov da uneti broj mora da bude veci od 1000
                   //i manji od 9999 da ne bi izlaz bio -1
      {
    printf("Rezultat je: -1\n");
    return -1;
  } //nije ispunjen uslov da broj bude u intervalu
      else {
    cifra0 = broj % 10; //svakoj cifri se pridruzuje vrednost, cifra0 najmanje
                        //tezine, zatim cifra1 ...
    cifra1 = (broj / 10) % 10;
    cifra2 = (broj / 100) % 10;
    cifra3 = broj / 1000;
    if (cifra0 % 2 == 0) //za svaku cifru proverava se da li je parna, ako jeste
                         //proizvod se mnozi sa tom cifrom
      proizvod *= cifra0;
    if (cifra1 % 2 == 0)
      proizvod *= cifra1;
    if (cifra2 % 2 == 0)
      proizvod *= cifra2;
    if (cifra3 % 2 == 0)
      proizvod *= cifra3;
    {
      printf("Rezultat je: %i\n", proizvod);
      return proizvod;
    } //stampa proizvod parnih cifara ako ih ima, ili 1 ako ih nema.
  }

  // return 0;
}

#include "../check.h"
