
#include <stdio.h>
#define max_size 21
int strcspn(char s[], char t[]) {
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
int main() {
  char a[max_size], b[max_size];
  scanf("%s", &a);
  scanf("%s", &b);
  printf("%d", strcspn(a, b));
  return 0;

}
