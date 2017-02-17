#include <stdio.h>
#define MAX 100

int indeks_maksimuma(float a[], int n);

int indeks_maksimumaR(float a[], int n) {
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

int assume(int);
int assert(int);

int main() {
  float a[MAX];
  int n, i;
  assume(n > 0 && n <= 100);

  int indeks1;
  int indeks2;

  indeks1 = indeks_maksimumaR(a, n);
  indeks2 = indeks_maksimuma(a, n);
  assert(indeks1 == indeks2);
}
