
#include <stdio.h>
float check_main(float a11, float a12, float a21, float a22) {
  float M;
  printf("Unesi brojeve koji ce biti clanovi matrice:\n");
  // scanf("%f%f%f%f", &a11, &a12, &a21, &a22);
  M = ((a11 * a22) - (a12 * a21));
  {
    printf("%1.4f\n", M);
    return M;
  }

  // return 0;
}

#include "../check.h"
