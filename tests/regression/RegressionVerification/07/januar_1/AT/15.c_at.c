
#include <stdio.h>

int check_main(float a, float b) {

  printf("unesi a: ");
  // scanf("%f",&a);
  printf("unesi b: ");
  // scanf("%f",&b);
  if (a > b) {
    {
      printf("1\n");
      return 1;
    }
  } else {
    {
      printf("2\n");
      return 2;
    }
  }

  // return 0;
}

#include "../check.h"
