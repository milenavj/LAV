#include "myfunctions.h"
#include <stdio.h>
#include <string.h>

int mystrcspn(char s[], char t[]) {

  int i, j, m, n;
  n = mystrlen(s);
  m = mystrlen(t);

  for (i = 0; i < n; i++) {
    for (j = 0; j < m; j++) { /*2 for-a zbog dve niske */
      if (s[i] == t[j]) {
        return i;
      }
    }
  }
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
