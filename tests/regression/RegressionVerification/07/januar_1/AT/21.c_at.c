
#include <stdio.h>

int check_main(float n, float m) {

  // scanf("%f%f",&n,&m);
  if (n > m) {
    printf("%d\n", 1);
    return 1;
  } else {
    printf("%d\n", 2);
    return 2;
  }

  // return 0;
}

#include "../check.h"
