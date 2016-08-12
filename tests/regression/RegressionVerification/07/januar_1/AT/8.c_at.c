
#include <stdio.h>
int check_main(float a, float b) {

  printf("\n Unesi dve vrednosti:");
  // scanf("%f%f",&a,&b);
  if (a > b) {
    printf("1");
    return 1;
  } else {
    printf("2");
    return 2;
  }
}

#include "../check.h"
