int __UF__5_1(char *, char *, int, int, int);
int __UF__4_0(char *, char *, int, int);

#define __RET_UNDEF -1
#include <stdio.h>

int mystrcspn(char s[], char t[]) {
  int i, j;
  i = 0;
  int __ret2065780014 = __RET_UNDEF;
  if ((s[i]) && (__ret2065780014 == __RET_UNDEF)) {
    j = 0;
    int __ret1289299011 = __RET_UNDEF;
    if ((t[j]) && (__ret1289299011 == __RET_UNDEF)) {
      if (s[i] == t[j])
        __ret1289299011 = i;
      if (__ret1289299011 == __RET_UNDEF) {

        j++;
      }
      __ret1289299011 = __UF__5_1(s, t, __ret1289299011, i, j);
    }
    if (!(__ret1289299011 == __RET_UNDEF))
      __ret2065780014 = __ret1289299011;
    if (__ret2065780014 == __RET_UNDEF) {

      ;
      i++;
    }
    __ret2065780014 = __UF__4_0(s, t, __ret2065780014, i);
  }
  if (!(__ret2065780014 == __RET_UNDEF))
    return __ret2065780014;

  ;
  return -1;
}

#if 0
int main() {
  char s[21], t[21];
  int g;
  scanf("%s", s);
  scanf("%s", t);
  g = mystrcspn(s, t);
  printf("%d\n", g);
  return 0;
}
#endif
#include "../check.h"
