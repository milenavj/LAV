
#include <stdio.h>

int check_main(float f1, float f2) {

  // scanf("%f%f",&f1,&f2);

  if (f1 > f2) {
    printf("1\n");
    return 1;
  } else {
    printf("2\n");
    return 2;
  }

  // return 0;
}

#include "../check.h"
