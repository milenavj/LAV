
#include <stdio.h>
#define MAX 21

int mystrcspn(char s[], char t[]) {
  int i, j;
  for (j = 0; s[j] != 0; j++) {
    for (i = 0; t[i] != 0; i++) {
      if (s[j] == t[i])
        return j;
    }
  }

  return -1;
}

#if 0
int main() {
  char s[MAX], t[MAX];
  scanf("%s\n", s);
  scanf("%s", t);

  printf("%d\n", mystrcspn(s, t));

  return 0;

}
#endif
#include "../check.h"
