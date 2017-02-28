#include <stdio.h>
#include <math.h>

int main() {
  int i;
  double a, b, c;
  scanf("%lf%lf", &a, &b);
  if (a < b) {
    c = (b - a) / 9;
    printf("%.4lf %.4lf %.4lf %.4lf %.4lf %.4lf %.4lf %.4lf %.4lf %.4lf\n",
           cos(a), cos(a + c), cos(a + 2 * c), cos(a + 3 * c), cos(a + 4 * c),
           cos(a + 5 * c), cos(a + 6 * c), cos(a + 7 * c), cos(a + 8 * c),
           cos(a + 9 * c));

  } else
    printf("%d\n", -1);
  return 0;
}
