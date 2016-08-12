#include <stdio.h>
#define MAX_NIZA 21
int izbaci(char a[], int k) {
  int i, brojac = 0;
  for (i = 0; a[i]; i++) //prebroj elemente niza
    brojac++;
  if (k < 0 || k >= brojac)
    return 0;
  for (i = k; i < brojac; i++) //krecuci od k, prepisivati vrednosti iz i+1 u i.
    a[i] = a[i + 1];
  return 1;

}

int main() {
  int poz, resenje;
  char rec[MAX_NIZA]; //maximum niza
  scanf("%d", &poz);  //unesi poziciju

  scanf("%s", rec); //unesi rec

  resenje = izbaci(
      rec, poz); //poziv funkcije i dodeljivanje rezultata promenljivoj resenje
  if (resenje == 0) //da li je resenje, da ne moze da se izvrsi izbacivanje
    printf("-1\n");
  else if (resenje == 1) //da li je resenje da moze da se izbaci
      {
    printf("1\n");
    printf("%s\n", rec); //stampaj rec
  }
  return 0;
}
