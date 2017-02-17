int __UF__5_0(float, float *, int, int, int);

#include <stdio.h>
#define MAX 100

int indeks_maksimuma(float a[], int n);

int indeks_maksimumaR(float a[], int n) {
  int i, m = 0;
  float max;
  max = a[0];
  i = 1;
  if (i < n) {
    if (a[i] >= max) {
      max = a[i];
      m = i;
    }
    i++;
    m = __UF__5_1(max, a, n, i, m);
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
