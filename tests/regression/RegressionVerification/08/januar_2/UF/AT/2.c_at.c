int __UF__5_1(char *, char *, int, int, int);
int __UF__4_0(char *, char *, int, int);

#define __RET_UNDEF -1
#include <stdio.h>

#define MAX 21

int mystrcspn(char s[], char t[]) {
  int i, j;
  i = 0;
  int __ret1598542192 = __RET_UNDEF;
  if ((s[i]) && (__ret1598542192 == __RET_UNDEF)) {
    j = 0;
    int __ret1912251162 = __RET_UNDEF;
    if ((t[j]) && (__ret1912251162 == __RET_UNDEF)) {
      if (s[i] == t[j])
        __ret1912251162 = i;
      if (__ret1912251162 == __RET_UNDEF) {

        j++;
      }
      __ret1912251162 = __UF__5_1(s, t, __ret1912251162, i, j);
    }
    if (!(__ret1912251162 == __RET_UNDEF))
      __ret1598542192 = __ret1912251162;
    if (__ret1598542192 == __RET_UNDEF) {

      ;
      i++;
    }
    __ret1598542192 = __UF__4_0(s, t, __ret1598542192, i);
  }
  if (!(__ret1598542192 == __RET_UNDEF))
    return __ret1598542192;

  ;          // vraca poziciju
  return -1; //ako ne nadje vraca -1
}

#if 0
int main() {
  char s[MAX], t[MAX];
  scanf("%s", s);
  scanf("%s", t);
  printf("%d\n", mystrcspn(s, t));
  return 0;
}
#endif
#include "../check.h"
