int __UF__5_1(char *, char *, int, int, int);
int __UF__4_0(char *, char *, int, int);

#define __RET_UNDEF -1
#include <stdio.h>
#define M 21
int mystrcspn(char s[], char t[]) {
  int i, j;
  i = 0;
  int __ret800003555 = __RET_UNDEF;
  if ((s[i]) && (__ret800003555 == __RET_UNDEF)) {
    j = 0;
    int __ret36287064 = __RET_UNDEF;
    if ((t[j]) && (__ret36287064 == __RET_UNDEF)) {
      if (s[i] == t[j])
        __ret36287064 = i;
      if (__ret36287064 == __RET_UNDEF) {

        j++;
      }
      __ret36287064 = __UF__5_1(s, t, __ret36287064, i, j);
    }
    if (!(__ret36287064 == __RET_UNDEF))
      __ret800003555 = __ret36287064;
    if (__ret800003555 == __RET_UNDEF) {

      ;
      i++;
    }
    __ret800003555 = __UF__4_0(s, t, __ret800003555, i);
  }
  if (!(__ret800003555 == __RET_UNDEF))
    return __ret800003555;

  ;

  return 22;
}

#if 0
int main() {
  char s[M];
  char t[M];
  int i;
  scanf("%s", &s);
  scanf("%s", &t);
  if (mystrcspn(s, t) == 22)
    printf("-1");
  else
    printf("%d", mystrcspn(s, t));
  return 0;
}
#endif
#include "../check.h"
