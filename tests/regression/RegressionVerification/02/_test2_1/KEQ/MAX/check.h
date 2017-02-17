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
  for (i = 1; i < n; i++)
    if (a[i] > max)
      max = a[i];
  return max;
}

int main() {
  float a[MAX_NIZ];
  int n;
  float max1, max2;

  assume(n > 0 && n <= 100);

  max1 = max_brojR(a, n);
  max2 = max_broj(a, n);
  assert(max1 == max2);

  return 0;
}
