
#include <stdio.h>
//definisemo maksimalnu duzinu niza
#define DIMENZIJA 100

//prototipovi funkcija
int indeks_minimuma(float a[], int n);
int indeks_maksimuma(float a[], int n);

#if 0
int main() {

  float a[DIMENZIJA];
  int n, i, min, max;
  //korisnik unosi dimenziju niza
  printf("Uneti dimenziju niza:  ");
  scanf("%d", &n);
  //ukoliko je korisnik uneo niz manji ili jednak nuli ili veci od max dimenzije
  //javljamo gresku!
  if (n <= 0 || n > DIMENZIJA) {
    printf("-1");
    return 0;
    printf("%d", min);
  }
  //korisnik unosi elemente niza
  printf("Uneti niz:  ");
  for (i = 0; i < n; i++)
    scanf("%f", &a[i]);

  //stampaju se rezultati
  printf("%d\n", indeks_minimuma(a, n));

  printf("%d\n", indeks_maksimuma(a, n));

  min = indeks_minimuma(a, n);
  max = indeks_maksimuma(a, n);
  //poredi vracene vrednosti prve i druge funkcije
  if (min > max)
    printf("1");
  else
    printf("0");

  return 0;
}
#endif

//funkcija trazi indeks minimuma
int indeks_minimuma(float a[], int n) {

  int i, b;
  float min;
  min = a[0];
  for (i = 0; i < n; i++)
    if (a[i] <= min) {
      min = a[i];
      b = i;
    }
  return b;

}

//funkcija trazi indeks maksimuma
int indeks_maksimuma(float a[], int n) {

  int i, b;
  float max;
  max = a[0];
  for (i = 0; i < n; i++)
    if (a[i] >= max) {
      max = a[i];
      b = i;
    }
  return b;

}
#include "../check.h"
