int __UF__5_1(char *, char *, int, int, int);
int __UF__5_0(char *, char *, int, int, int);

#define __RET_UNDEF -1

#include <stdio.h>
#define max_size 21
int mystrcspn(char s[], char t[]) {
  int i, j;
  i = 0;
  j = 0;
  int __ret1334736679 = __RET_UNDEF;
  if ((s[i]) && (__ret1334736679 == __RET_UNDEF)) {
    int __ret571429534 = __RET_UNDEF;
    if ((t[j]) && (__ret571429534 == __RET_UNDEF)) {
      if (s[i] == t[j])
        __ret571429534 = i;
      if (__ret571429534 == __RET_UNDEF) {

        j++;
      }
      __ret571429534 = __UF__5_1(s, t, __ret571429534, i, j);
    }
    if (!(__ret571429534 == __RET_UNDEF))
      __ret1334736679 = __ret571429534;
    if (__ret1334736679 == __RET_UNDEF) {

      j = 0;
      i++;
    }
    __ret1334736679 = __UF__5_0(s, t, __ret1334736679, i, j);
  }
  if (!(__ret1334736679 == __RET_UNDEF))
    return __ret1334736679;

  return -1;
}

#if 0
int main() {
  char a[max_size], b[max_size];
  scanf("%s", &a);
  scanf("%s", &b);
  printf("%d", mystrcspn(a, b));
  return 0;

}
#endif
#include "../check.h"
