int result_main(int x) {

  int p, h, s, d, j;
  if (x < 1000 || x > 9999)
    return -1;
  else {
    h = x / 1000;
    s = (x / 100) % 10;
    d = (x / 10) % 10;
    j = x % 10;
    if (h % 2 != 0)
      h = 1;
    if (s % 2 != 0)
      s = 1;
    if (d % 2 != 0)
      d = 1;
    if (j % 2 != 0)
      j = 1;
    p = h * s * d * j;
    return p;
  }
} /* Ovde se zavrsava program */

int assert(int);
int main() {
  int n;
  printf("Unesite pozitivan cetvorocifren broj: ");
      /* Stampam poruku da treba da se unese broj */
  scanf("%d", &n); /* Ucitavam broj */
  assert(result_main(n) == check_main(n));
}
