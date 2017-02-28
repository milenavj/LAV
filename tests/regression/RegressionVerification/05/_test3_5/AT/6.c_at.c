
#include <stdio.h>
#define max 21
int izbaci(char c[max], int k) {
  int i, n = 0;
  while (c[n] != '\0')
    n++;
  if ((k > n - 1) || (k < 0))
    return 0; // Funkcija za izbacivanje karaktera
  for (i = 0; i < n; i++) {
    if (i == k)
      for (i = k; i < n; i++) {
        c[i] = c[i + 1];
      }
  }
  return 1;
}

#if 0
int main() {
  char c[max];
  int n, i, d = 0, p;
  printf("Unesi poziciju : ");
  scanf("%d", &n); // Unos pozicije i reci
  printf("Unesi rec : ");
  scanf("%s", c);
  p = izbaci(c, n);
  if (p == 0)
    printf("\n-1");
  else {
    printf("\n1 : "); // Provera i stampa
    printf("%s", c);
  }

  return 0;
}
#endif
#include "check.h"
