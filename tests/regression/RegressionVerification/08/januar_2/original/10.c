#include <stdio.h>

#define MAX_ARR 20

int strcspn(char s[], char t[]) {
  int i, j;
  for (i = 0; s[i] != '\0'; i++) {
    for (j = 0; t[j] != '\0'; j++) {
      if (s[i] == t[j]) {
        return i;
      }
    }
  }

  return -1;
}

int main() {
  char s[MAX_ARR + 1];
  char t[MAX_ARR + 1];
  int pozicija = 0;

  printf("unesi prvu nisku: ");
  scanf("%s", &s);
  printf("unesi drugu nisku: ");
  scanf("%s", &t);
  pozicija = strcspn(s, t);
  printf("%d\n", pozicija);

  return 0;

}
