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

  char a[21], b[21];

  scanf("%s", a);
  printf("\n");
  scanf("%s", b);

  printf("%d\n", strcspn(a, b));

  return 0;

}
