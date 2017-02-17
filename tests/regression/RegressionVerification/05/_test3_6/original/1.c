#include <stdio.h>
#define MAX 22
#include <string.h>

int ubaci(char rec[], int k, int n) {
  int i;

  if (n > 20 || n <= k || k < 0)
    return 0;
  else {
    rec[n + 1] = '\0';
    for (i = n; i >= k; i--)
      rec[i + 1] = rec[i];
  }
  return 1;
}

int main() {
  char c[MAX];
  int k, n, x;
  printf("unesite poziciju karaktera\n");
  scanf("%d", &k);
  printf("unesite nisku\n");
  scanf("%s", &c);

  n = strlen(c);
  x = ubaci(c, k, n);

  if (x == 1) {
    printf("%d\n", 1);
    printf("%s\n", c);
  } else
    printf("%d\n", -1);
  return 1;

  return 0;
}
