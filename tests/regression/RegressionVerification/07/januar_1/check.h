int result_main(float a, float b) {
  if (a > b)
    return 1;
  else
    return 2;
}
int assert(int);
int main() {
  float a, b;
  scanf("%f%f", &a, &b);
  assert(result_main(a, b) == check_main(a, b));
  return 0;
}
