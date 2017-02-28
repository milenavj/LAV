#include <stdio.h>
#include <math.h>

int main() {

  double a, b, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, x;

  scanf("%lf%lf", &a, &b);
  if (a >= b) {
    printf("-1\n");
    return -1;
  }
  x = (b - a) / 9;
  t1 = a;
  t2 = t1 + x;
  t3 = t2 + x;
  t4 = t3 + x;
  t5 = t4 + x;
  t6 = t5 + x;
  t7 = t6 + x;
  t8 = t7 + x;
  t9 = t8 + x;
  t10 = t9 + x;

  printf("%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n", cos(t1), cos(t2), cos(t3),
         cos(t4), cos(t5), cos(t6), cos(t7), cos(t8), cos(t9), cos(t10));

  return 0;
}
