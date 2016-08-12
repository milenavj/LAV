int __UF__5_1(char *, char *, int, int, int);
int __UF__4_0(char *, char *, int, int);

#define __RET_UNDEF -1
#include <stdio.h>
#define MAX 21

int mystrcspn(char s[], char t[]) {
  int i, j;
  j = 0;
  int __ret1954960360 = __RET_UNDEF;
  if ((s[j] != 0) && (__ret1954960360 == __RET_UNDEF)) {
    i = 0;
    int __ret117287224 = __RET_UNDEF;
    if ((t[i] != 0) && (__ret117287224 == __RET_UNDEF)) {
      if (s[j] == t[i])
        __ret117287224 = j;
      if (__ret117287224 == __RET_UNDEF) {
        ;
        i++;
      }
      __ret117287224 = __UF__5_1(s, t, __ret117287224, i, j);
    }
    if (!(__ret117287224 == __RET_UNDEF))
      __ret1954960360 = __ret117287224;
    if (__ret1954960360 == __RET_UNDEF) {

      j++;
    }
    __ret1954960360 = __UF__4_0(s, t, __ret1954960360, j);
  }
  if (!(__ret1954960360 == __RET_UNDEF))
    return __ret1954960360;

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
