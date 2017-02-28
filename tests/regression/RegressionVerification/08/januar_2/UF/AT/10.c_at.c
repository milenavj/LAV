int __UF__5_1(char *, char *, int, int, int);
int __UF__4_0(char *, char *, int, int);

#define __RET_UNDEF -1
#include <stdio.h>

#define MAX_ARR 20

int mystrcspn(char s[], char t[]) {
  int i, j;
  i = 0;
  int __ret912581680 = __RET_UNDEF;
  if ((s[i] != '\0') && (__ret912581680 == __RET_UNDEF)) {
    j = 0;
    int __ret1205132286 = __RET_UNDEF;
    if ((t[j] != '\0') && (__ret1205132286 == __RET_UNDEF)) {
      if (s[i] == t[j]) {
        __ret1205132286 = i;
      }
      if (__ret1205132286 == __RET_UNDEF) {

        j++;
      }
      __ret1205132286 = __UF__5_1(s, t, __ret1205132286, i, j);
    }
    if (!(__ret1205132286 == __RET_UNDEF))
      __ret912581680 = __ret1205132286;
    if (__ret912581680 == __RET_UNDEF) {

      i++;
    }
    __ret912581680 = __UF__4_0(s, t, __ret912581680, i);
  }
  if (!(__ret912581680 == __RET_UNDEF))
    return __ret912581680;

  return -1;
}

#if 0
int main() {
  char s[MAX_ARR + 1];
  char t[MAX_ARR + 1];
  int pozicija = 0;

  printf("unesi prvu nisku: ");
  scanf("%s", &s);
  printf("unesi drugu nisku: ");
  scanf("%s", &t);
  pozicija = mystrcspn(s, t);
  printf("%d\n", pozicija);

  return 0;

}
#endif
#include "../check.h"
