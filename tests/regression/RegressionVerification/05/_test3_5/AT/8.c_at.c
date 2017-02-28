
#include <stdio.h>

#define MAX 21

int izbaci(char s[], int k) {
  int br = 0, i = 0, n = 0, j;
  while (s[i] != '\0') {
    br++;
    i++;
  }
  if (k >= br || k < 0)
    return 0;
  else {
    for (j = k; s[j] != '\0'; j++)
      s[j] = s[j + 1];
    return 1;
  }
}

#if 0
int main() {
  char s[MAX];
  int k, i = 0;
  printf("Unesi poziciju:\n");
  scanf("%d", &k);
  printf("Unesi rec:\n");
  scanf("%s", s);
  if (izbaci(s, k) == 0 || sizeof(s) > MAX)
    printf("-1\n");
  else {
    printf("1\n");
    printf("%s\n", s);
  }

  return 0;
}
#endif
#include "check.h"
