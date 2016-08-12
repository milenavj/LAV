int __UF__5_0(float, float *, int, int, int);

#include <stdio.h>
#define MAX 100

int indeks_minimuma(float a[], int n);

int indeks_minimumaR(float a[], int n) {
  int i, m = 0;
  float min;
  min = a[0];
  i = 1;
  if (i < n) {
    if (a[i] <= min) {
      min = a[i];
      m = i;
    }
    i++;
    m = __UF__5_0(min, a, n, i, m);
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

  indeks1 = indeks_minimumaR(a, n);
  indeks2 = indeks_minimuma(a, n);
  assert(indeks1 == indeks2);

}
