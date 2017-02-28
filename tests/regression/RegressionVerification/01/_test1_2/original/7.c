#include <stdio.h>
int main() {
  int n, a1, a2, a3, a4, p;

  printf("Unesite pozitivan cetvorocifren broj n:");
  scanf("%d", &n);

  p = 1; //proizvod

  if (n >= 1000 && n <= 9999) {
    a1 = n % 10; //poslednja cifra
    a2 = n / 10 % 10;
    a3 = n / 100 % 10;
    a4 = n / 1000; //prva cifra

    if (a1 % 2 == 0)
      p = p * a1;
    if (a2 % 2 == 0)
      p = p * a2;
    if (a3 % 2 == 0)
      p = p * a3;
    if (a4 % 2 == 0)
      p = p * a4;
    if (p != 1)
      printf("proizvod parnih cifara je P= %d. \n", p);
    else
      printf("%d \n", p);
  } else
    printf("-1 \n");
  return 0;
}
