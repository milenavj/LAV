int __UF__5_1(char *, char *, int, int, int);
int __UF__4_0(char *, char *, int, int);

#define __RET_UNDEF -1
#include <stdio.h>
#include <string.h>
#define SIZE 21

int mystrcspn(char s[], char t[]) {
  int i, j;
  i = 0;
  int __ret929632610 = __RET_UNDEF;
  if ((s[i]) && (__ret929632610 == __RET_UNDEF)) {
    j = 0;
    int __ret164684985 = __RET_UNDEF;
    if ((t[j]) && (__ret164684985 == __RET_UNDEF)) {
      if (s[i] == t[j]) //prolazimo kroz obe niske i uporedjujemo clanove
        __ret164684985 = i;
      if (__ret164684985 == __RET_UNDEF) {

        j++;
      }
      __ret164684985 = __UF__5_1(s, t, __ret164684985, i, j);
    }
    if (!(__ret164684985 == __RET_UNDEF))
      __ret929632610 = __ret164684985;
    if (__ret929632610 == __RET_UNDEF) {

      ;
      i++;
    }
    __ret929632610 = __UF__4_0(s, t, __ret929632610, i);
  }
  if (!(__ret929632610 == __RET_UNDEF))
    return __ret929632610;

  ; //
  return -1;
}

#if 0
int main() {
  char t[SIZE], s[SIZE]; //deklaracija niski

  scanf("%s", s); //unos prve niske
  scanf("%s", t); //unos druge niske

  printf("%d\n", mystrcspn(s, t)); //stampanje vrednosti koje vraca funkcija

  return 0;
}
#endif
#include "../check.h"
