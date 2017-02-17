#include <stdio.h>
#include <math.h>
int main() {
  double a, b, c;
  printf("Unesi a i b:");
  scanf("%lf%lf", &a, &b);
  if (a < b) {
    c = (b - a) / 9;
    double i = a;
    for (i; i <= b + 0.000001; i += c)
      printf("%.4lf ", cos(i));
  } else
    printf("-1\n");
}
