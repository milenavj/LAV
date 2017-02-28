#include <stdio.h>
#define MAX 100

int indeks_minimuma(float a[], int n) {
  int i, m = 0;
  float min;
  min = a[0];
  for (i = 1; i < n; i++)
    if (a[i] <= min) {
      min = a[i];
      m = i;
    }
  return m;
}

int indeks_maksimuma(float a[], int n) {
  int i, m = 0;
  float max;
  max = a[0];
  for (i = 1; i < n; i++)
    if (a[i] >= max) {
      max = a[i];
      m = i;
    }
  return m;
}

int main() {
  float a[MAX];
  int i, n, min, max;
  printf("Uneti dimenziju niza: ");
  scanf("%d", &n);
  if (n < 0 || n > 100) {
    printf("Izlaz:\n");
    printf("-1\n");
    return 0;
  }
  printf("Uneti niz: ");
  for (i = 0; i < n; i++)
    scanf("%f", &a[i]);
  printf("Izlaz:\n");
  min = indeks_minimuma(a, n);
  max = indeks_maksimuma(a, n);
  printf("%d\n", min);
  printf("%d\n", max);
  if (min > max)
    printf("1\n");
  else
    printf("0\n");
  return 0;
}
