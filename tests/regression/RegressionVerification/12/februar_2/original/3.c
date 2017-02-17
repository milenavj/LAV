#include <stdio.h>
int main() {
  int a, b, c, max;
  scanf("%d", &a);
  scanf("%d", &b);
  scanf("%d", &c);
  max = a;
  if (b > max && b > c)
    max = b;
  else if (c > max)
    max = c;
  printf("%d\n", max);
  return 0;
}
