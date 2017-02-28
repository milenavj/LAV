int __UF__5_0(char *, char *, int, int, int);

#define __RET_UNDEF -1
#include <stdio.h>
#define MAX_SIZE 21

int mystrcspn(char s[], char t[]) {
  int i, j, br;
  i = 0;
  int __ret1510607574 = __RET_UNDEF;
  if ((s[i]) && (__ret1510607574 == __RET_UNDEF)) {
    j = 0;
    int __ret1811412362 = __RET_UNDEF;
    while ((t[j]) && (__ret1811412362 == __RET_UNDEF)) {
      if (s[i] == t[j]) {
        br = i;
        __ret1811412362 = br;
      }
      if (__ret1811412362 == __RET_UNDEF) {

        j++;
      }
    }
    if (!(__ret1811412362 == __RET_UNDEF))
      __ret1510607574 = __ret1811412362;
    if (__ret1510607574 == __RET_UNDEF) {

      i++;
    }
    __ret1510607574 = __UF__5_0(s, t, __ret1510607574, br, i);
  }
  if (!(__ret1510607574 == __RET_UNDEF))
    return __ret1510607574;

  return -1;
}

#if 0
int main() {
  char s[MAX_SIZE];
  char t[MAX_SIZE];
  int i = 0, j = 0, br;
  printf("Unesi s: ");
  scanf("%s", &s);
  printf("Unesi t: ");
  scanf("%s", &t);

  br = mystrcspn(s, t);
  printf("%d\n", br);

  return 0;
}
#endif
#include "../check.h"
