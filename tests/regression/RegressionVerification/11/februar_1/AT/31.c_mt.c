#include <stdio.h>
#define size 10

int check_main(float a11, float a12, float a21, float a22) {

  float a[size][size], b;
  int i, j;

  printf("Unesite brojeve\n");

  //for(i=0;i<2;i++)
  //  for(j=0;j<2;j++)
  //    scanf("%f",&a[i][j]);
  a[0][0] = a11;
  a[0][1] = a12;
  a[1][0] = a21;
  a[1][1] = a22;

  b = a[0][0] * a[1][1] - a[0][1] * a[1][0];

  printf("%.4f", b);
  return b;
}
