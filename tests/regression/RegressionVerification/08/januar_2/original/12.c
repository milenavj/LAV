#include <stdio.h>

int strcspn(char s[], char t[]) {
  int i, j;
  for (i = 0; s[i]; i++)
    for (j = 0; t[j]; j++)
      if (s[i] == t[j])
        return i;
  return -1;
}

int main() {
  char s[21], t[21];
  int g;
  scanf("%s", s);
  scanf("%s", t);
  g = strcspn(s, t);
  printf("%d\n", g);
  return 0;
}
