

#include <stdio.h>
#define max_size 21
int mystrcspn(char s[], char t[]) {
  int i, j;
  i = 0;
  j = 0;
  while (s[i]) {
    while (t[j]) {
      if (s[i] == t[j])
        return i;
      j++;
    }
    j = 0;
    i++;
  }
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
