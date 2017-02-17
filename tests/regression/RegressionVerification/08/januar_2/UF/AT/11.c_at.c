int __UF__5_1(char *, char *, int, int, int);
int __UF__4_0(char *, char *, int, int);

#define __RET_UNDEF -1
#include <stdio.h>
#define max_size 21

int mystrcspn(char s[], char t[]) {
  int i, j;
  i = 0;
  int __ret1258692318 = __RET_UNDEF;
  if ((s[i]) && (__ret1258692318 == __RET_UNDEF)) {
    j = 0;
    int __ret1565641343 = __RET_UNDEF;
    if ((t[j]) && (__ret1565641343 == __RET_UNDEF)) {
      if (s[i] == t[j])
        __ret1565641343 = i;
      if (__ret1565641343 == __RET_UNDEF) {

        j++;
      }
      __ret1565641343 = __UF__5_1(s, t, __ret1565641343, i, j);
    }
    if (!(__ret1565641343 == __RET_UNDEF))
      __ret1258692318 = __ret1565641343;
    if (__ret1258692318 == __RET_UNDEF) {

      ;
      i++;
    }
    __ret1258692318 = __UF__4_0(s, t, __ret1258692318, i);
  }
  if (!(__ret1258692318 == __RET_UNDEF))
    return __ret1258692318;

  ;
  return -1;
}

#if 0
int main() {
  char s[max_size], t[max_size];
  scanf("%s", s);
  scanf("%s", t);
  printf("%d\n", mystrcspn(s, t));
  return 0;
}
#endif
#include "../check.h"
