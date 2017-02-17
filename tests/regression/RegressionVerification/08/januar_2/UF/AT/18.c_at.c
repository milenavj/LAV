int __UF__5_1(char *, char *, int, int, int);
int __UF__4_0(char *, char *, int, int);

#define __RET_UNDEF -1
#include <stdio.h>

int mystrcspn(char s[], char t[]) {
  int i, j;
  i = 0;
  int __ret1625074068 = __RET_UNDEF;
  if ((s[i]) && (__ret1625074068 == __RET_UNDEF)) {
    j = 0;
    int __ret856697080 = __RET_UNDEF;
    if ((t[j]) && (__ret856697080 == __RET_UNDEF)) {
      if (s[i] == t[j]) //ako nadje jedan...
        __ret856697080 = i;
      if (__ret856697080 == __RET_UNDEF) {

        j++;
      }
      __ret856697080 = __UF__5_1(s, t, __ret856697080, i, j);
    }
    if (!(__ret856697080 == __RET_UNDEF))
      __ret1625074068 = __ret856697080;
    if (__ret1625074068 == __RET_UNDEF) {

      ; //vraca trenutnu poziciju u niski s kao resenje
        //dolazak dovde znaci da trenutni karakter niske s nije u niski t;
      i++;
    }
    __ret1625074068 = __UF__4_0(s, t, __ret1625074068, i);
  }
  if (!(__ret1625074068 == __RET_UNDEF))
    return __ret1625074068;

  //do ovde se dolazi samo ako nijedan karakter iz s nije sadrzan u t
  return -1; //u tom slucaju je -1 resenje
}

#if 0
void unos_niske(char s[]) //pomocna f-ja za unos niske do novog reda
    {
  int i = 0;
  char c = getchar();
  while (c != '\n' && c != '\r' && c != EOF) {
    s[i++] = c;
    c = getchar();
  }
  s[i] = 0; //ne sme se zaboraviti null-terminator!
}

//gets u string ubacuje i \n sto nije dobro, a scanf("%s") cita do bilo
//koje praznine, a ne samo do kraja reda. zato sam napisao ovu f-ju.
#endif

#if 0
int main() {
  char s[21], t[21]; //20 (max) karaktera + null za kraj stringa

  unos_niske(s);
  unos_niske(t);

  //fprintf(stderr, "%s\n", s);
  //fprintf(stderr, "%s\n", t);

  printf("%d\n", mystrcspn(s, t)); //poziv f-je

  return 0;
}
#endif
#include "../check.h"
