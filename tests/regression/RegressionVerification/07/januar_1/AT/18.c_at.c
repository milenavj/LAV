
#include <stdio.h>
int check_main(float a, float b) {

  printf("Uneisite a i b\n");
  // scanf("%f%f",&a,&b);
  if (a > b) {
    printf("1");
    return 1;
  } else {
    printf("2");
    return 2;
  } //Poredjenje brojeva
    // return 0;
}

#include "../check.h"
