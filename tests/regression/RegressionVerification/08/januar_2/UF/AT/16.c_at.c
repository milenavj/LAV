int __UF__5_1(char *, char *, int, int, int);
int __UF__4_0(char *, char *, int, int);

#include "myfunctions.h"
#define __RET_UNDEF -1
#include <stdio.h>
#define size 21     // +1 za '/0'
#include <string.h> // za mystrlen

int mystrcspn(char s[], char t[]) {
  int i, j;

  i = 0;
  int __ret1096177204 = __RET_UNDEF;
  if ((i < mystrlen(s)) && (__ret1096177204 == __RET_UNDEF)) {
    j = 0;
    int __ret326053913 = __RET_UNDEF;
    if ((j < mystrlen(t)) && (__ret326053913 == __RET_UNDEF)) {
      if (s[i] == t[j])
        __ret326053913 = i;
      if (__ret326053913 == __RET_UNDEF) {

        j++;
      }
      __ret326053913 = __UF__5_1(s, t, __ret326053913, i, j);
    }
    if (!(__ret326053913 == __RET_UNDEF))
      __ret1096177204 = __ret326053913;
    if (__ret1096177204 == __RET_UNDEF) {

      ;
      i++;
    }
    __ret1096177204 = __UF__4_0(s, t, __ret1096177204, i);
  }
  if (!(__ret1096177204 == __RET_UNDEF))
    return __ret1096177204;

  ; // poredim i-ti iz s sa svima iz t, pa i+1...
  return -1;
}

#if 0
int main() {
  char s[size], t[size];
  int a;

  printf("Unesite prvu nisku:\n");
  scanf("%s", &s);

  printf("Unesite drugu nisku:\n");
  scanf("%s", &t);

  a = mystrcspn(s, t);

  printf("%d\n", a);

  return 0;
}
#endif
#include "../check.h"
