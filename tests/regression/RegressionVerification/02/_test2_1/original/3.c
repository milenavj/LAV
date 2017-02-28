#include <stdio.h>
#define MAX_SIZE 100

float max_broj(float a[], int n) {
  int i;
  float max;
  max = a[0];
  for (i = 0; i < n; i++) {
    if (a[i] > max)
      max = a[i];
  }
  return max;
}
float aritmeticka_sredina(float a[], int n) {
  int i;
  float s = 0, ar;
  for (i = 0; i < n; i++) {
    s = s + a[i];
  }
  ar = s / n;
  return ar;
}
int main() {
  int n, i;
  float ar, max;
  float a[MAX_SIZE];
  printf("Uneti dimenziju niza:");
  scanf("%d", &n);
  if ((n <= 0) || (n > MAX_SIZE))
    printf("-1\n");
  else {
    printf("Uneti niz:");
    for (i = 0; i < n; i++)
      scanf("%f", &a[i]);
    printf("Izlaz:\n");
    printf("%f\n", max_broj(a, n));
    printf("%f\n", aritmeticka_sredina(a, n));
    max = max_broj(a, n);
    ar = aritmeticka_sredina(a, n);
    if (max > (2 * ar))
      printf("1\n");
    else
      printf("0\n");
  }
  return 0;
}
