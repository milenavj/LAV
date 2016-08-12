
#include <stdio.h>
#define MAXN 100

float max_broj(float a[], int n) {
  float max = 0;
  int i;
  for (i = 0; i < n; i++) {
    if (a[i] > max) {
      max = a[i];
    }
  }
  return max;
}

float aritmeticka_sredina(float a[], int n) {
  float s = 0;
  int i;
  for (i = 0; i < n; i++) {
    s += a[i];
  }
  return s / n;
}

#if 0
int main() {
  float a[MAXN];
  int n;
  printf("Uneti dimenziju niza: ");
  scanf("%d", &n);
  if (n < 1 || n > MAXN) //ne postoji aritmeticka sredina nijednog clana
                         //niza,niti postoji maksimum
      {
    printf("-1\n");
    return 0;
  }
  int i;
  printf("Uneti niz: ");
  for (i = 0; i < n; i++) {
    scanf("%f", &a[i]);
  }
  float maxb = max_broj(a, n);
  float ars = aritmeticka_sredina(a, n);
  printf("%f\n%f\n%i\n", maxb, ars, maxb > ars * 2);
      /*da program ne bi 2 puta izvrsavao istu funkciju pamtimo rezultat
funkcija u promenljive koje kasnije koristimo u uslovu treceg ispisa */
  return 0;
}
#endif
#include "../check.h"
