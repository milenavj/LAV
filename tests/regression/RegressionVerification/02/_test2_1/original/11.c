#include <stdio.h>
#define MAX_SIZE 100
#if 0
void ucitaj(float a[], int n) /* funkcija za ucitavanje niza */
    {
  int i;
  printf("Ucitaj clanove niza: ");
  for (i = 0; i < n; i++) /* petlja kojom se ucitavaju clanovi niza */
    scanf("%f", &a[i]);

}
#endif

float max_broj(float a[], int n) /* funkcija koja trazi maksimum niza*/
    {
  int i;
  float max = a[0];       /*promenljiva koja cuva maksimum niza*/
  for (i = 1; i < n; i++) /*petlja kojom se pronalazi maksimum niza*/
    if (a[i] > max)
      max = a[i];
  return max; /*povratna vrednost funkcije*/

}
float aritmeticka_sredina(float a[], int n) {
  int i;
  float suma = 0.0;
  for (i = 0; i < n; i++) /*petlja koja racuna aritmeticku sredinu*/
    suma = suma + a[i];

  return suma / (float) n;
}

int main() {
  int n;
  float a[MAX_SIZE];
  printf("Unesite dimenziju niza: ");
  scanf("%d", &n);
  if (n <= 0 || n > MAX_SIZE) {
    printf("-1\n");
    return 0;
  }             /* uslov nepravilnog unosa*/
  ucitaj(a, n); /*ucitava niz*/
  printf("\n");
  printf("%f\n", max_broj(a, n));            /*stampa maksimum*/
  printf("%f\n", aritmeticka_sredina(a, n)); /*stampa arit.sredinu*/
  if (max_broj(a, n) > (2 * aritmeticka_sredina(a, n)))
    printf("1\n"); /* uslov da je max veci od arit.sredine*/
  else
    printf("0\n");

  return 0;
}
