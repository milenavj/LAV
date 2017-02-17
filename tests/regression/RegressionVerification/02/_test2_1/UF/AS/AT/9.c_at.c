float __UF__4_1(float, float *, int, int);
float __UF__4_0(float, float *, int, int);

#include <stdio.h>

#define MAX 100

float max_broj(float a[], int n);
float aritmeticka_sredina(float a[], int n);

float max_broj(float a[], int n) {
  int i;
  float m = a[0];
  i = 0;
  if (i < n) {
    if (a[i] > m)
      m = a[i];
    i++;
    m = __UF__4_0(m, a, n, i);
  }
  ; /* ova for petlja trazi maksimum  */
  return m;
}

float aritmeticka_sredina(float a[], int n) {
  int i;
  float s = 0.0;
  i = 0;
  if (i < n) {
    s += a[i];
    i++;
    s = __UF__4_1(s, a, n, i);
  }
  ;          /* izracunava sumu svih clanova niza */
  s = s / n; /*deli tu sumu sa brojem clanova */
  return s;
}

#if 0
int main() {
  float niz[MAX];
  int i, n;
  float m, a;
  /*	printf("Unesite broj elemenata(n<100) \n");*/
  scanf("%d", &n);
  if ((n > 100) || (n < 1))
    printf("-1"); /*ako je n neodgovarajuce ispisuje -1 */
  else {
    /*			printf("Unesite %d elemenata niza\n",n);*/
    for (i = 0; i < n; i++)
      scanf("%f", &niz[i]); /* ucitava niz */
    m = max_broj(niz, n);
        /*ove dve linije koda pozivaju funkcije za ucitani niz */
    a = aritmeticka_sredina(niz, n);
    printf("%f\n", m);
    printf("%f\n", a); /*ispisuje rezultat */
    if (m > 2 * a)
      printf("1\n");
    else
      printf("0\n");
  }

  return 0;
}
#endif
#include "../check.h"
