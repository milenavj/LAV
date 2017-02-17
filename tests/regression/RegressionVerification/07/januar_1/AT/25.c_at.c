
#include <stdio.h>

int check_main(float a, float b) {

  // scanf("%f%f", &a, &b);
  if (a > b) {
    printf("%d\n", 1);
    return 1;
  } else {
    printf("%d\n", 2);
    return 2;
  }

  // return 0;
}

#include "../check.h"
