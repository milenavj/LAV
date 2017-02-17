int assert(int);
int result_main(int sat, int minut, int sekund) {
  if (sat < 0 || sat > 23 || minut < 0 || minut > 59 || sekund < 0 ||
      sekund > 59)
    return -1;

  if (sat < 12)
    return (12 - sat) * 3600 - minut * 60 - sekund;
  else
    return 12 * 3600 + (24 - sat) * 3600 - minut * 60 - sekund;
}

int main() {
  int sat, minut, sekund;
  long int ps;

  scanf("%d %d %d", &sat, &minut, &sekund);

  assert(result_main(sat, minut, sekund) == check_main(sat, minut, sekund));
  return 0;
}
