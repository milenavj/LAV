int __UF__6_1(char *, char *, int, int, int, int);
int __UF__6_0(char *, char *, int, int, int, int);

#include "myfunctions.h"
#define __RET_UNDEF -1
#include <stdio.h>
#include <string.h>

int mystrcspn(char s[], char t[]) {

  int i, j, m, n;
  n = mystrlen(s);
  m = mystrlen(t);

  i = 0;
  int __ret13486013 = __RET_UNDEF;
  if ((i < n) && (__ret13486013 == __RET_UNDEF)) {
    j = 0;
    int __ret1386729745 = __RET_UNDEF;
    if ((j < m) && (__ret1386729745 == __RET_UNDEF)) {
      /*2 for-a zbog dve niske */
      if (s[i] == t[j]) {
        __ret1386729745 = i;
      }
      if (__ret1386729745 == __RET_UNDEF) {

        j++;
      }
      __ret1386729745 = __UF__6_1(s, t, __ret1386729745, i, j, m);
    }
    if (!(__ret1386729745 == __RET_UNDEF))
      __ret13486013 = __ret1386729745;
    if (__ret13486013 == __RET_UNDEF) {

      i++;
    }
    __ret13486013 = __UF__6_0(s, t, __ret13486013, i, m, n);
  }
  if (!(__ret13486013 == __RET_UNDEF))
    return __ret13486013;

  return -1;
}

#if 0
int main() {
  char s[21], t[21];
  int n;

  printf("Unesite dve niske: ");
  scanf("%s%s", s, t); /*unosimo niske */
  n = mystrcspn(s, t); /*pozivamo funkciju i povratnu vrednost zapisujemo u n */
  printf("%d\n", n);

}
#endif
#include "../check.h"
