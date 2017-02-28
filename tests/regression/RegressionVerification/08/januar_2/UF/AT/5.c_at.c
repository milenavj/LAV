int __UF__5_1(char *, char *, int, int, int);
int __UF__4_0(char *, char *, int, int);

#include "myfunctions.h"
#define __RET_UNDEF -1
#include <stdio.h>
#include <string.h>
#define SIZE 21
int mystrcspn(char rec[], char rec1[]) {
  int i, j;
  i = 0;
  int __ret2055046192 = __RET_UNDEF;
  if ((rec[i]) && (__ret2055046192 == __RET_UNDEF)) {
    j = 0;
    int __ret202918424 = __RET_UNDEF;
    if ((rec1[j]) && (__ret202918424 == __RET_UNDEF)) {
      if (rec1[j] == rec[i])
        __ret202918424 = i;
      if (__ret202918424 == __RET_UNDEF) {

        j++;
      }
      __ret202918424 = __UF__5_1(rec, rec1, __ret202918424, i, j);
    }
    if (!(__ret202918424 == __RET_UNDEF))
      __ret2055046192 = __ret202918424;
    if (__ret2055046192 == __RET_UNDEF) {

      ;
      i++;
    }
    __ret2055046192 = __UF__4_0(rec, rec1, __ret2055046192, i);
  }
  if (!(__ret2055046192 == __RET_UNDEF))
    return __ret2055046192;

  ;
  return -1;
}

#if 0
main() {
  char rec[SIZE], rec1[SIZE];
  printf("Unesite prvu rec: ");
  scanf("%s", rec);
  printf("Unesite drugu rec: ");
  scanf("%s", rec1);
  if (mystrlen(rec) > SIZE || mystrlen(rec1) > SIZE)
    return 0;
  if (mystrcspn(rec, rec1))
    printf("%d\n", mystrcspn(rec, rec1));
  else if (mystrcspn(rec, rec1) == -1)
    printf("-1\n");
  else
    printf("0\n");

}
#endif
#include "../check.h"
