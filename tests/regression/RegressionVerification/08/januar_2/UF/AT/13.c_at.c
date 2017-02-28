int __UF__5_1(char *, char *, int, int, int);
int __UF__4_0(char *, char *, int, int);

#define __RET_UNDEF -1
#include <stdio.h>
#define maxNiza 21
int mystrcspn(char s[], char t[]) {
  int i, j;
  i = 0;
  int __ret1062974876 = __RET_UNDEF;
  if ((s[i]) && (__ret1062974876 == __RET_UNDEF)) {
    j = 0;
    int __ret283088151 = __RET_UNDEF;
    if ((t[j]) && (__ret283088151 == __RET_UNDEF)) {
      if (s[i] == t[j])
        __ret283088151 = i;
      if (__ret283088151 == __RET_UNDEF) {

        j++;
      }
      __ret283088151 = __UF__5_1(s, t, __ret283088151, i, j);
    }
    if (!(__ret283088151 == __RET_UNDEF))
      __ret1062974876 = __ret283088151;
    if (__ret1062974876 == __RET_UNDEF) {

      ;
      i++;
    }
    __ret1062974876 = __UF__4_0(s, t, __ret1062974876, i);
  }
  if (!(__ret1062974876 == __RET_UNDEF))
    return __ret1062974876;

  ;
  return -1;
}

#if 0
int main() {
  int rezultat;
  char s[maxNiza], t[maxNiza];
  scanf("%s", s);
  scanf("%s", t);
  rezultat = mystrcspn(s, t);
  if (rezultat != -1)
    printf("%d\n", rezultat);
  else
    printf("%d\n", rezultat);

  return 0;
}
#endif
#include "../check.h"
