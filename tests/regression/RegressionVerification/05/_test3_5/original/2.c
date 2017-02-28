#include <stdio.h>
#define SIZE 20

int izbaci(char s[], int k) {
  int i, n = 0;
  while (s[n])
    n++;
  if (k < 0 || k >= SIZE)
    return 0;
  else
    for (i = k; s[i + 1] != '\0'; i++)
      s[i] = s[i + 1];
  s[i] = '\0';
  return 1;
}

int main() {
  int k, n = 0;
  char s[SIZE];

  printf("Unesi poziciju:\n");
  scanf("%d", &k);
  printf("Unesi rec:\n");
  scanf("%s", s);

  while (s[n])
    n++;
  if (k < 0 || k > n - 1)
    printf("-1\n");
  else if (izbaci(s, k) == 1) {
    printf("1\n");
    printf("%s\n", s);
  } else
    printf("-1\n");

  return 0;
}
