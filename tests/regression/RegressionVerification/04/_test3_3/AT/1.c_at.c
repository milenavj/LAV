
#include <stdio.h>

#define MAX 10

/* a)   potrebno je zadati i dimenzije matrice */
void maksimumi(int A[][MAX], int m, int n, int out[]) {
  int i, j, max;
  for (i = 0; i < m; i++) { /* za svaku vrstu uzimamo maksimum */
    max = A[i][0];          /*uzmemo prvi element kao najveci */
    for (j = 1; j < n; j++) /*i za svaki sledeci */
      if (A[i][j] > max)    /*proveravamo da li je veci */
        max = A[i][j];      /*i menjamo maksimum ako jeste */
    out[i] = max;           /*na kraju maksimum te vrste ubacimo u niz */
  }
}

/* b) */

#if 0
int main() {
  int m, n, i, j;
  int a[MAX][MAX], nizmax[MAX];

  /* Unos podataka! */
  printf("Unesi broj vrsta: ");
  scanf("%d", &m);
  printf("Unesi broj kolona: ");
  scanf("%d", &n);
  if (m < 1 || n < 1 || m > MAX || n > MAX) { /* provera ispravnosti unosa */
    printf("-1\n");                           /* greska */
    return 1;
  }
  printf("Unesi elemente matrice:\n");
  for (i = 0; i < m; i++)
    for (j = 0; j < n; j++)
      scanf("%d", &a[i][j]); /* moze i a[i]+j jer je pokazivac */

  maksimumi(a, m, n, nizmax);
      /* poziv funkcije za popunjavajne niza maksimuma */
  for (i = 0; i < m; i++) /* ispis niza maksimuma */
    printf("%d ", nizmax[i]);
  printf("\n");

  return 0;
}
#endif
#include "check.h"
