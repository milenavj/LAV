#include <stdio.h>

int main() {
  float n, m;
  scanf("%f%f", &n, &m);
  if (n > m)
    printf("%d\n", 1);
  else
    printf("%d\n", 2);

  return 0;
}
