#include <stdio.h>
float maksimum(float a[], float n);
float aritmetickasr(float a[], float n);
int main() {
  float a[100];
  int i, n;
  printf("unesi dimenziju niza: ");
  printf("\n");
  scanf("%d", &n);
  if (n < 1 || n > 100)
    printf("-1\n");
  else {
    for (i = 0; i < n; i++)
      scanf("%f", &a[i]);
    printf("%f\n", maksimum(a, n));
    printf("%f\n", aritmetickasr(a, n));
    if (maksimum(a, n) > 2 * aritmetickasr(a, n))
      printf("1\n");
    else
      printf("0\n");
  }
}
float maksimum(float a[], float n) {
  int i;
  float maxelement;
  maxelement = a[0];
  for (i = 0; i < n; i++)
    if (maxelement < a[i])
      maxelement = a[i];
  return maxelement;
}

float aritmetickasr(float a[], float n) {
  float S = 0, Ar;
  int i;
  for (i = 0; i < n; i++) {
    S += a[i];
  }
  Ar = S / n;
  return Ar;
}
