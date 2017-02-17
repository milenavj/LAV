#include <stdio.h>
#define MAX 100

int indeks_minimuma(float a[], int n);

int indeks_minimumaR(float a[], int n) {
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
