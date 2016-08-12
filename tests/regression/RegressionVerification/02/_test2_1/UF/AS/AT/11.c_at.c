float __UF__4_1(float, float *, int, int);
float __UF__4_0(float, float *, int, int);

#include <stdio.h>
#define MAX_SIZE 100
#if 0
void ucitaj(float a[], int n) { /* funkcija za ucitavanje niza */
  int i;
  printf("Ucitaj clanove niza: ");
  for (i = 0; i < n; i++) /* petlja kojom se ucitavaju clanovi niza */
    scanf("%f", &a[i]);

}
#endif

float max_broj(float a[], int n) { /* funkcija koja trazi maksimum niza */
  int i;
  float max = a[0]; /*promenljiva koja cuva maksimum niza */
  i = 1;
  if (i < n) {
    if (a[i] > max)
      max = a[i];
    i++;
    max = __UF__4_0(max, a, n, i);
  }
  ;
  return max; /*povratna vrednost funkcije */

}

float aritmeticka_sredina(float a[], int n) {
  int i;
  float suma = 0.0;
  i = 0;
  if (i < n) {
    suma = suma + a[i];
    i++;
    suma = __UF__4_1(suma, a, n, i);
  }
  ;

  return suma / (float) n;
}

#if 0
int main() {
  int n;
  float a[MAX_SIZE];
  printf("Unesite dimenziju niza: ");
  scanf("%d", &n);
  if (n <= 0 || n > MAX_SIZE) {
    printf("-1\n");
    return 0;
  }             /* uslov nepravilnog unosa */
  ucitaj(a, n); /*ucitava niz */
  printf("\n");
  printf("%f\n", max_broj(a, n));            /*stampa maksimum */
  printf("%f\n", aritmeticka_sredina(a, n)); /*stampa arit.sredinu */
  if (max_broj(a, n) > (2 * aritmeticka_sredina(a, n)))
    printf("1\n"); /* uslov da je max veci od arit.sredine */
  else
    printf("0\n");

  return 0;
}
#endif
#include "../check.h"
