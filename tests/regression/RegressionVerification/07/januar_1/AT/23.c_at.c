
#include <stdio.h>

int check_main(float x, float y) {

  // scanf("%f%f",&x,&y);
  if (x > y) {
    printf("1");
    return 1;
  } else {
    printf("2");
    return 2;
  }

  // return 0;
}

#include "../check.h"
