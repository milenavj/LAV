
#include <stdio.h>
int check_main(float a, float b) {

  printf("unesi prvi broj:");
  // scanf("%f",&a);
  printf("unesi drugi broj:");
  // scanf("%f",&b);
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
