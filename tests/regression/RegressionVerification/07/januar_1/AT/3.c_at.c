
#include <stdio.h>

int check_main(float a, float b) {

  int max;

  // scanf("%f %f", &a, &b);

  max = (a > b) ? 1 : 2;

  {
    printf("%d\n", max);
    return max;
  }

  // return 0;
}

#include "../check.h"
