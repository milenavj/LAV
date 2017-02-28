#include <stdio.h>
#define max 10

void zbir(float a[][max], int n, float *sum) {
  int i, j;
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      if ((j + i) > (n - 1))
        *sum += a[i][j]; //sabira elemente ispod sporedne dijagonale//
}

int main() {
  float sum = 0, a[max][max];
  int i, j, n;
  printf("Uneti ceo broj:\n");
  scanf("%d", &n);
  if (n <= 0 || n > max) {
    printf("-1\n");
    return 0;
  } //uneta dimenzija se nalazi van zadate//
  printf("Uneti matricu:\n");
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      scanf("%f", &a[i][j]);
  zbir(a, n, &sum);
  printf("%.3f\n", sum); //zbir elemenata ispod sporedne dijagonale//
  return 0;
}
