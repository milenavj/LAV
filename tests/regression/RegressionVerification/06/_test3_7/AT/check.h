
#include <stdio.h>
#ifndef MAX
#define MAX 10
#endif
int assert(int);

int main() {
  float a[MAX][MAX], sum = 0, sum1 = 0;
  int n, i, j;
  scanf("%d", &n);
  if (n <= MAX && n > 0) {
    for (i = 0; i < n; i++)
      for (j = 0; j <= n - 2 - i; j++)
        sum += a[i][j];

    zbir(a, n, &sum1);
    assert(sum == sum1);
  }
  return 0;
}
