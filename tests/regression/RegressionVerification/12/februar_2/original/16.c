#include <stdio.h>

main() {

  int a, b, c, pom;
  scanf("%d %d %d", &a, &b, &c);
  if (a > b && a > c)
    pom = a;
  else if (b > c)
    pom = b;
  else
    pom = c;

  printf("%d \n", pom);

  return 0;
}
