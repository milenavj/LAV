
#include <stdio.h>

int check_main(float a, float b) {

  // scanf("%f%f",&a,&b);
  if (a > b) {
    printf("1");
    return 1;
  } else {
    printf("2");
    return 2;
  }
  // return 0;
}

#include "../check.h"
