int assert(int);

int result_main(double a, double b, double result[]) {
  double c;
  if (a < b) {
    c = (b - a) / 9;
    result[0] = cos(a);
    result[1] = cos(a + c);
    result[2] = cos(a + 2 * c);
    result[3] = cos(a + 3 * c);
    result[4] = cos(a + 4 * c);
    result[5] = cos(a + 5 * c);
    result[6] = cos(a + 6 * c);
    result[7] = cos(a + 7 * c);
    result[8] = cos(a + 8 * c);
    result[9] = cos(a + 9 * c);
  } else
    result[0] = -1;
  return 0;
}

int main() {
  double result1[10];
  double result2[10];
  int i;
  double a, b, c, d;
  scanf("%lf%lf", &a, &b);
  result_main(a, b, result1);
  check_main(a, b, result2);
  if (a < b)
    for (i = 0; i < 10; i++) {
      c = result1[i];
      d = result2[i];
      assert(c == d);
    }
  else {
    c = result1[0];
    d = result2[0];
    assert(c == d);
  }
  return 0;
}
