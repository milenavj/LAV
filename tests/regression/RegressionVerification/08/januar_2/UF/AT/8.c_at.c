int __UF__5_1(char *, char *, int, int, int);
int __UF__4_0(char *, char *, int, int);

#define __RET_UNDEF -1
#include <stdio.h>

int mystrcspn(char s[], char t[]) {
  int i, j;
  i = 0;
  int __ret175918291 = __RET_UNDEF;
  if ((s[i] != '\0') && (__ret175918291 == __RET_UNDEF)) {
    j = 0;
    int __ret476284737 = __RET_UNDEF;
    if ((t[j] != '\0') && (__ret476284737 == __RET_UNDEF)) {
      if (s[i] == t[j])
        __ret476284737 = i;
      if (__ret476284737 == __RET_UNDEF) {
        ;
        j++;
      }
      __ret476284737 = __UF__5_1(s, t, __ret476284737, i, j);
    }
    if (!(__ret476284737 == __RET_UNDEF))
      __ret175918291 = __ret476284737;
    if (__ret175918291 == __RET_UNDEF) {

      i++;
    }
    __ret175918291 = __UF__4_0(s, t, __ret175918291, i);
  }
  if (!(__ret175918291 == __RET_UNDEF))
    return __ret175918291;

  return -1;
}

#if 0
int main() {
  char prva[21], druga[21];
  int rez;

  scanf("%s", prva);
  scanf("%s", druga);

  rez = strscpn(prva, druga);

  if (rez >= 0)
    printf("%d\n", rez);
  else
    printf("-1\n");
  return 0;
}
#endif
#include "../check.h"
