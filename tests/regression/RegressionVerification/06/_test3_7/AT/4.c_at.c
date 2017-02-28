
#include <stdio.h>

#define max 10

void zbir(float a[][max], int n, float *sum) {
  *sum = 0.0;
  int i, j, k;
  k = n;

  for (i = 0; i < (n - 1); i++) {
    for (j = 0; j < (k - 1); j++)
      *sum += a[i][j];
    k--;
  }

}

#if 0
int main() {
  float a[max][max];
  float sum;
  int n, i, j;

  printf("Uneti broj vrsta i kolona matrice: ");
  scanf("%d", &n);

  if (1 > n || 10 < n)
    printf("-1\n");
  else {
    printf("Uneti elemente matrice:\n");
    for (i = 0; i < n; i++)
      for (j = 0; j < n; j++)
        scanf("%f", &a[i][j]);

    zbir(a, n, &sum);
    printf("Zbir brojeva iznad sporedne dijagonale je: %f\n", sum);
  }

  return 0;
}
#endif
#include "check.h"
