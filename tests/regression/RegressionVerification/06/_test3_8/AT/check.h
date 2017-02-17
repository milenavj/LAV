#include <stdio.h>
#ifndef max
#define max 10
#endif
int assert(int);

int main() {
  float a[max][max], sum = 0, sum1 = 0;
  int n, i, j;

  if (n <= 0 || n > max) {
    return -1;
    sum = -1;
  } else {
    for (i = 0; i < n; i++)
      for (j = 0; j < n; j++)
        if ((j + i) > (n - 1))
          sum += a[i][j];
    zbir(a, n, &sum1);
    assert(sum1 == sum);
  }

  return 0;
}
