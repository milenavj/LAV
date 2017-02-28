
float result_main(float a11, float a12, float a21, float a22) {
  return a11 * a22 - a12 * a21;
}

int assert(int);

int main() {
  float a11, a12, a21, a22;
  scanf("%f%f%f%f", &a11, &a12, &a21, &a22);
  assert(result_main(a11, a12, a21, a22) == check_main(a11, a12, a21, a22));
  return 0;
}
