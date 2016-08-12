
#include <stdio.h>
#define M 21
int mystrcspn(char s[], char t[]) {
  int i, j;
  for (i = 0; s[i]; i++)
    for (j = 0; t[j]; j++)
      if (s[i] == t[j])
        return i;

  return 22;
}

#if 0
int main() {
  char s[M];
  char t[M];
  int i;
  scanf("%s", &s);
  scanf("%s", &t);
  if (mystrcspn(s, t) == 22)
    printf("-1");
  else
    printf("%d", mystrcspn(s, t));
  return 0;
}
#endif
#include "../check.h"
