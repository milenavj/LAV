
#include <stdio.h>
int check_main(int a, int b, int c) {
  int max;
  //  printf("Unesi 3 broj:\n");
  // scanf ("%d%d%d", &a, &b, &c);
  max = a;
  if (b > max)
    max = b;
  if (c > max)
    max = c;
  {
    printf("max = %d\n", max);
    return max;
  }
}

#include "../check.h"
