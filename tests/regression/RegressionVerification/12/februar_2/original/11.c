#include <stdio.h>

int main() {
  int a, b, c;

  scanf("%d", &a);
  scanf("%d", &b);
  scanf("%d", &c);
  if (a > b) {
    if (a > c)
      printf("%d", a);
    else if (c > b)
      printf("%d", c);
  } else if (c > a) {
    if (c > b)
      printf("%d", c);
    else if (b > c)
      printf("%d", b);
  } else
    printf("%d\n", b);

  return 0;

}
