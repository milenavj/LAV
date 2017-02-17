int __UF__5_1(char *, char *, int, int, int);
int __UF__4_0(char *, char *, int, int);

#include "myfunctions.h"
#define __RET_UNDEF -1
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int mystrcspn(char s[], char t[]) {
  int i, j;
  i = 0;
  int __ret422010867 = __RET_UNDEF;
  if ((i < mystrlen(s)) && (__ret422010867 == __RET_UNDEF)) {
    j = 0;
    int __ret1809032135 = __RET_UNDEF;
    if ((j < mystrlen(t)) && (__ret1809032135 == __RET_UNDEF)) {
      if (s[i] == t[j])
        __ret1809032135 = i;
      if (__ret1809032135 == __RET_UNDEF) {
        ;
        j++;
      }
      __ret1809032135 = __UF__5_1(s, t, __ret1809032135, i, j);
    }
    if (!(__ret1809032135 == __RET_UNDEF))
      __ret422010867 = __ret1809032135;
    if (__ret422010867 == __RET_UNDEF) {

      i++;
    }
    __ret422010867 = __UF__4_0(s, t, __ret422010867, i);
  }
  if (!(__ret422010867 == __RET_UNDEF))
    return __ret422010867;

  return -1;
}

#if 0
int main() {
  char s[21];
  char t[21];
  scanf("%s", s);
  scanf("%s", t);
  printf("\n%d", mystrcspn(s, t));
}
#endif
#include "../check.h"
