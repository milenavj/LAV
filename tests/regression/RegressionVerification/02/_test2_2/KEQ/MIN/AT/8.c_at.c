
#include <stdio.h>
#define MAX_SIZE 100

#if 0
void unesi(float niz[], int n) {
  int i;
  for (i = 0; i < n; i++)
    scanf("%f", &niz[i]); //UNOSI SE NIZ
}
#endif

int indeks_minimuma(float niz[], int n) {
  int i, j; // INDEKSI SU INTEGER A MIN JE FLOAT JER JE I NIZ FLOAT
  float min;
  min = niz[0];

  for (i = 0; i < n; i++) {
    if (niz[i] <= min) {
      min = niz[i];
      j = i;
    }
  }
  return j;
}

int indeks_maksimuma(float niz[], int n) {
  int i, j; // INDEKSI SU INTEGER A MAX JE FLOAT JER JE I NIZ FLOAT
  float max;
  max = niz[0];

  for (i = 0; i < n; i++) {
    if (niz[i] >= max) {
      max = niz[i];
      j = i;
    }
  }
  return j;
}

#if 0
int main() {
  float niz[MAX_SIZE];
  int n, min, max;

  printf("Unesite dimenziju niza: ");
  scanf("%d", &n);

  if (n <= 0 || n > MAX_SIZE) {
    printf("-1\n");
    return 0;
  }
  unesi(niz, n); //pozivam funkicju unos

  min = indeks_minimuma(
      niz, n); // min dobija vrednost koju vraca funkcija indeks_minimum
  max = indeks_maksimuma(
      niz, n); // max dobija vrednost koju vraca funkcija indeks_minimum

  printf("%d \n", min);
  printf("%d\n", max);

  if (min > max)
    printf("1\n");
  else
    printf("0\n");
  return 0;
}
#endif
#include "../check.h"
