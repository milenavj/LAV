float __UF__4_0(float, float *, int, int);

#include <stdio.h>
#ifndef MAX_NIZ
#define MAX_NIZ 100
#endif

int assert(int);
int assume(int);

float max_broj(float a[], int n);

float max_brojR(float a[], int n) {
  int i;
  float max = a[0];
  i = 1;
  if (i < n) {
    if (a[i] > max)
      max = a[i];
    i++;
    max = __UF__4_0(max, a, n, i);
  }
  ;
  return max;
}

main() {
  float a[MAX_NIZ];
  int n;
  float max1, max2;

  assume(n > 0 && n <= 100);

  max1 = max_brojR(a, n);
  max2 = max_broj(a, n);
  assert(max1 == max2);

  return 0;
}
