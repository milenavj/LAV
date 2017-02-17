int __UF__5_1(char *, char *, int, int, int);
int __UF__4_0(char *, char *, int, int);

#define __RET_UNDEF -1
#include <stdio.h>

int mystrcspn(char s[], char t[]) {
  int i, j;
  i = 0;
  int __ret1922873808 = __RET_UNDEF;
  if ((s[i]) && (__ret1922873808 == __RET_UNDEF)) {
    j = 0;
    int __ret1148367445 = __RET_UNDEF;
    if ((t[j]) && (__ret1148367445 == __RET_UNDEF)) {
      if (s[i] == t[j])
        __ret1148367445 = i;
      if (__ret1148367445 == __RET_UNDEF) {

        j++;
      }
      __ret1148367445 = __UF__5_1(s, t, __ret1148367445, i, j);
    }
    if (!(__ret1148367445 == __RET_UNDEF))
      __ret1922873808 = __ret1148367445;
    if (__ret1922873808 == __RET_UNDEF) {

      ;
      i++;
    }
    __ret1922873808 = __UF__4_0(s, t, __ret1922873808, i);
  }
  if (!(__ret1922873808 == __RET_UNDEF))
    return __ret1922873808;

  ;
  return -1;
}

#if 0
int main() {
  char s[21], t[21];
  scanf("%s", &s);
  scanf("%s", &t);
  printf("%d\n", mystrcspn(s, t));

  return 0;
}
#endif
#include "../check.h"
