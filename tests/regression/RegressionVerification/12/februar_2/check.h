#include <stdio.h>
int assert(int);

int result_main(int a, int b, int c) {
  int max;
  max = a;
  if (b > max)
    max = b;
  if (c > max)
    max = c;
  return max;
}

int main() {
  int a, b, c;
  scanf("%d %d %d", &a, &b, &c);
  assert(result_main(a, b, c) == check_main(a, b, c));
  return 0;
}
