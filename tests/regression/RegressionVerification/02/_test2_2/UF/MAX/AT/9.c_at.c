int __UF__5_1(float, float *, int, int, int);
int __UF__5_0(float, float *, int, int, int);

#define __RET_UNDEF -1
#include <stdio.h>
#define broj 100

int indeks_minimuma(float a[], int n); //deklaracije funkcija
int indeks_maksimuma(float a[], int n);
void ucitajNiz(float a[], int n);

#if 0
int main() {

  float a[broj];
  int n, min, max;
  printf("Uneti dimenziju niza: ");
  scanf("%d", &n);

  if (n < 1 || n > broj) { //ako je broj elemenata niza manji od nule napusti
                           //izvrsavanje programa
    printf("Izlaz:\n-1\n");
    return 0;
  }

  printf("Uneti niza: ");
  ucitajNiz(a, n);

  printf("Izlaz:\n");

  min = indeks_minimuma(a, n);  //vraca indeks minimuma
  max = indeks_maksimuma(a, n); //vraca indeks maksimuma

  printf("%d\n", min);

  printf("%d\n", max);

  if (min > max)
    printf("1\n"); //stampa 1 ako je indeks minimuma veci od indeksa maksimuma,
                   //inace stampa 0
  else
    printf("0\n");

}
#endif

int indeks_minimuma(float a[], int n) {

  int i, ind;
  float min;

  min = a[0]; //neka je za pocetak minimum prvi element
  ind = 0;
  i = 1;
  if (i < n) {
    if (a[i] <= min) { // ako je naredni manji ili jednak trenutnom min postaje
                       // taj i uzima njegovu poziciju
      min = a[i]; //mora biti uslov manje ili jednako da bi se dobio indeks
                  //poslednjeg minimuma
      ind = i;
    }
    i++;
    ind = __UF__5_0(min, a, n, i, ind);
  }

  return ind;

}

int indeks_maksimuma(float a[],
                     int n) { //za maksimum su slicni komentari kao i za minimum
  int i, ind;
  float max;

  max = a[0];
  ind = 0;

  i = 1;
  if (i < n) {
    if (a[i] >= max) {
      max = a[i];
      ind = i;
    }
    i++;
    ind = __UF__5_1(max, a, n, i, ind);
  }

  return ind;

}

#if 0
void ucitajNiz(float a[], int n) {

  int i;
  for (i = 0; i < n; i++)
    scanf("%f", &a[i]);
}
#endif
#include "../check.h"
