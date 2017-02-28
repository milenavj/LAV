#include <stdio.h>
#include <string.h>
#define size 21

int assume(int);
int assert(int);

void izmeniR(char rec[]) {
  int i;
  for (i = 0; rec[i]; i++)
    if (i % 2 == 0) {
      if (rec[i] >= 'a' && rec[i] <= 'z')
        rec[i] = rec[i] - 'a' + 'A';
    } else if (rec[i] >= 'A' && rec[i] <= 'Z')
      rec[i] = rec[i] - 'A' + 'a';
}

int main() {
  char a[size];
  char b[size];
  int i, n;
  char c;
  printf("Unesi rec: ");
  scanf("%s", a);
  for (i = 0; a[i]; i++)
    b[i] = a[i];

  izmeni(b);
  izmeniR(a);

  for (i = 0; a[i]; i++)
    assert(b[i] == a[i]);

  return 0;
}
