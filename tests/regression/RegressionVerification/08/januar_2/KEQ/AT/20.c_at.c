
#include <stdio.h>
#include <string.h>
#include "myfunctions.h"

int mystrcspn(char s[], char t[]) {
  int n, m, i, j, p = -1;
  n = mystrlen(s);
  m = mystrlen(t);
  for (i = 0; i < n; i++) // Funkcija za proveru
    for (j = 0; j < m; j++) {
      if (s[i] == t[j]) {
        p = i;
        return p;
      }
    }
  return p;
}

#if 0
int main() {
  char s[21], t[21];
  int p;
  printf("Unesite prvu nisku\n");
  scanf("%s", s);
  printf("Unesite drugu nisku\n"); //Unos niski
  scanf("%s", t);
  p = mystrcspn(s, t);
  printf("\n%d", p);
  return 0;
}
#endif
#include "../check.h"
