
#ifndef MYFUNCTIONS
#define MYFUNCTIONS
int mystrlen(char *a) {
  int i = 0;
  while (a[i])
    i++;
  return i;
}

int myabs(int x) { return x > 0 ? x : -x; }
#endif
