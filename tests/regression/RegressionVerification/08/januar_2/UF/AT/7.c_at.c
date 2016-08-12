int __UF__5_1(char *, char *, int, int, int);
int __UF__4_0(char *, char *, int, int);

#define __RET_UNDEF -1
#include <stdio.h>

int mystrcspn(char s[], char t[]) {

  int i, j;
  i = 0;
  int __ret1616326413 = __RET_UNDEF;
  if ((s[i]) && (__ret1616326413 == __RET_UNDEF)) {
    j = 0;
    int __ret1915080993 = __RET_UNDEF;
    if ((t[j]) && (__ret1915080993 == __RET_UNDEF)) {
      if (s[i] == t[j])
        __ret1915080993 = i;
      if (__ret1915080993 == __RET_UNDEF) {

        j++;
      }
      __ret1915080993 = __UF__5_1(s, t, __ret1915080993, i, j);
    }
    if (!(__ret1915080993 == __RET_UNDEF))
      __ret1616326413 = __ret1915080993;
    if (__ret1616326413 == __RET_UNDEF) {

      ;
      i++;
    }
    __ret1616326413 = __UF__4_0(s, t, __ret1616326413, i);
  }
  if (!(__ret1616326413 == __RET_UNDEF))
    return __ret1616326413;

  ;
  return -1;
}

#if 0
int main() {

  char a[21], b[21];

  scanf("%s", a);
  printf("\n");
  scanf("%s", b);

  printf("%d\n", mystrcspn(a, b));

  return 0;

}
#endif
#include "../check.h"
