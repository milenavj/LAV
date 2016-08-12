float __UF__4_1(float, float *, int, int);
float __UF__4_0(float, float *, int, int);

#include <stdio.h>
#define max_size 100

float max_broj(float a[], int na) { /*funkcija za maximum niza */
  float max; /*promenljiva u koju se smesta maksimum i koju funkcija vraca */
  int i;
  max = a[0];
  i = 1;
  if (i < na) {
    /*petlja u kojoj se odredjuje maximum niza */

    if (a[i] > max)
      max = a[i];
    ;
    i++;
    max = __UF__4_0(max, a, na, i);
  }

  return max;
}

float aritmeticka_sredina(float a[], int na) {
  /*funkcija u kojoj se racuna aritmeticka sredina */

  float s, arsr; /* s je promeljiva za zbir svih clanova niza, a arsr je
promenljiva za aritmeticku sredinu */
  s = 0;
  int k;
  k = 0;
  if (k < na) {
    s = s + a[k];
    k++;
    s = __UF__4_1(s, a, na, k);
  }
  ;

  arsr = s / (float) na; /*racuna se aritmeticka sredina */
  return arsr;
}

#if 0
int main() {

  float niz[max_size];
  int n, j;

  printf("Unesi n\n");
  scanf("%d", &n);

  if (n > max_size || n < 0) { /*provera dimenzije niza */

    printf("-1\n");
    return 0;

  }
  if (n == 0) { /*proverava se da li niz ima bar jedan clan */
    printf("Niz je prazan, nema nikakvih clanova pa se ne moze naci ni max ni "
           "aritmeticka sredina\n");
    return 0;
  }

  for (j = 0; j < n; j++) { /*punjenje niza */
    printf("Unesi clan\n");
    scanf("%f", &niz[j]);
  }
  /*stampanje*/
  printf("%f\n", max_broj(niz, n));
  printf("%f\n", aritmeticka_sredina(niz, n));
  if (max_broj(niz, n) > 2 * (aritmeticka_sredina(niz, n)))
    printf("1\n");
  else
    printf("0\n");

  return 0;
}
#endif
#include "../check.h"
