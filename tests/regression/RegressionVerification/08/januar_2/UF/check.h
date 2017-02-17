int __UF__5_1(char *, char *, int, int, int);
int __UF__4_0(char *, char *, int, int);

#define __RET_UNDEF -1
#include <stdio.h>

#define MAX_ARR 20

int assert(int);

int mystrcspn(char s[], char t[]);

int mystrcspnR(char s[], char t[]) {
  int i, j;
  i = 0;
  int __ret578577392 = __RET_UNDEF;
  if ((s[i] != '\0') && (__ret578577392 == __RET_UNDEF)) {
    j = 0;
    int __ret1957542314 = __RET_UNDEF;
    if ((t[j] != '\0') && (__ret1957542314 == __RET_UNDEF)) {
      if (s[i] == t[j]) {
        __ret1957542314 = i;
      }
      if (__ret1957542314 == __RET_UNDEF) {

        j++;
      }
      __ret1957542314 = __UF__5_1(s, t, __ret1957542314, i, j);
    }
    if (!(__ret1957542314 == __RET_UNDEF))
      __ret578577392 = __ret1957542314;
    if (__ret578577392 == __RET_UNDEF) {

      i++;
    }
    __ret578577392 = __UF__4_0(s, t, __ret578577392, i);
  }
  if (!(__ret578577392 == __RET_UNDEF))
    return __ret578577392;

  return -1;
}

int main() {
  char s[MAX_ARR + 1];
  char t[MAX_ARR + 1];
  int pozicijaR = 0, pozicija = 0;

  printf("unesi prvu nisku: ");
  scanf("%s", s);
  printf("unesi drugu nisku: ");
  scanf("%s", t);
  pozicijaR = mystrcspnR(s, t);
  pozicija = mystrcspn(s, t);
  assert(pozicijaR == pozicija);

  return 0;
}
