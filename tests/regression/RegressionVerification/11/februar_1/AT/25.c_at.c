
#include <stdio.h>

float check_main(float a11, float a12, float a21, float a22) {
  float rez;

  //printf("unesite a11:\n");     /*Pre svega unosimo sve brojeve koji se traze
  //u zadatku*/
  // scanf("%f", &a11);

  //printf("unesite a12:\n");
  // scanf("%f", &a12);

  //printf("unesite a21:\n");
  // scanf("%f", &a21);

  //printf("unesite a22:\n");
  // scanf("%f", &a22);

  rez = a11 * a22 - a12 * a21;

  {
    printf("%.4f\n", rez);
    return rez;
  } /*Pisemo %.4f, kako bi smo dobili ispisivanje samo 4 decimale */

  // return 0;
}

#include "../check.h"
