float __UF__4_1(float, float *, int, int);
float __UF__4_0(float, float *, int, int);

#include <stdio.h>
#define BROJELEMENATA 100
/* simbolicka konstanta za maximalan broj elemenata niza */

float max_broj(float a[], int n);

float aritmeticka_sredina(float a[], int n); /* deklaracije funkcija */

float max_broj(float a[], int n) {
  int i;
  float max;
  max = a[0]; /*maximum postavljen kao prvi element */
  i = 1;
  if (i < n) {
    if (a[i] > max) {
      max = a[i];
          /* ukoliko je neki element veci od maximuma on postaje maximum */
    }
    i++;
    max = __UF__4_0(max, a, n, i);
  }

  return max;
}

float aritmeticka_sredina(float a[], int n) {
  float zb = 0, ars;
  int i;
  i = 0;
  if (i < n) {
    zb = zb + a[i]; /* zbir elemenata */
    ;
    i++;
    zb = __UF__4_1(zb, a, n, i);
  }

  ars = (float) zb / n; /* racunanje aritmeticke sredine */

  return ars;

}

#if 0
int main() {
  int n, i;
  float a[BROJELEMENATA], j, k;
  printf("Uneti dimenziju niza: ");
  scanf("%d", &n);
  if (n <= 0 || n > BROJELEMENATA) {
        /* ispitivanje da li je dobro unesen broj elemenata */
    printf("Izlaz: \n -1\n");
        /* ukoliko br. elemenata nije dobro unesen ispis -1 */
  } else {
    printf("Uneti niz: ");
    for (i = 0; i < n; i++)
      scanf("%f", &a[i]); /* unos niza */
    printf("Izlaz: \n");
    printf("%f\n", max_broj(a, n));            /* ispis maximuma */
    printf("%f\n", aritmeticka_sredina(a, n)); /* ispis ar. sredine */
    j = max_broj(a, n);
    k = aritmeticka_sredina(a, n) * 2;
    if (j > k) /* ispitivanje da li je maximum dvostruko strogo veci od ar.
sredine */
      printf("1\n");
    else
      printf("0\n");
  }

  return 0;
}
#endif
#include "../check.h"
