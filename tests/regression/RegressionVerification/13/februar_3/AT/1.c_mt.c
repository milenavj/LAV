#include <stdio.h>
#include <math.h>
int check_main(double a, double b, double result[]) {
  //  printf("Unesi a i b:");
  //  scanf("%lf%lf",&a,&b);
  double c;
  if (a < b) {
    c = (b - a) / 9;
    int k = 0;
    double i = a;
    for (i; i <= b + 0.000001; i += c) {
      printf("%.4lf ", cos(i));
      result[k++] = cos(i);
    }
  } else {
    printf("-1\n");
    result[0] = -1;
  }
  return 0;
}

#include "../check.h"
