
#include <stdio.h>
int check_main(float a, float b) {

  printf("Unesite prvi broj: ");
  // scanf("%f",&a);
  printf("Unesite drugi broj: ");
  // scanf("%f",&b);
  if (a > b) {
    {
      printf("1\n");
      return 1;
    }
    // return 0;
  } else {
    {
      printf("2\n");
      return 2;
    }
    // return 0;
  }
}

#include "../check.h"
