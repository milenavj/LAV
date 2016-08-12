float __UF__4_1(float, float *, int, int);
float __UF__4_0(float, float *, int, int);

#include <stdio.h>
#define MAX_SIZE 100

float max_broj(float a[], int n) {
  int i;
  float max;
  max = a[0];
  i = 0;
  if (i < n) {
    if (a[i] > max)
      max = a[i];
    ;
    i++;
    max = __UF__4_0(max, a, n, i);
  }

  return max;
}

float aritmeticka_sredina(float a[], int n) {
  int i;
  float s = 0, ar;
  i = 0;
  if (i < n) {
    s = s + a[i];
    ;
    i++;
    s = __UF__4_1(s, a, n, i);
  }

  ar = s / n;
  return ar;
}

#if 0
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
#endif
#include "../check.h"
