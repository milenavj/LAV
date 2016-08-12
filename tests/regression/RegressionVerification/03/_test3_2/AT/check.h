#include "myfunctions.h"
#include <stdio.h>
#define size 21

int assume(int);
int assert(int);

void izmeniR(char a[]) {
  int i;
  for (i = 0; i < mystrlen(a); i++) {
    if (i % 3 == 0)
      a[i] = toupper(a[i]);
    if (i % 3 == 1)
      a[i] = tolower(a[i]);
  }
}

int main() {
  char a[size];
  char b[size];
  int i, n;
  char c;
  printf("Unesi rec: ");
  scanf("%s", a);
  n = mystrlen(a);
  for (i = 0; i < n; i++)
    b[i] = a[i];

  izmeni(b);
  izmeniR(a);

  for (i = 0; i < n; i++)
    assert(b[i] == a[i]);

  return 0;
}
