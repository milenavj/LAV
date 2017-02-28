#include "myfunctions.h"
#include <stdio.h>
#include <string.h>
#define MAX_NIZA 21

int assert(int);
int main() {
  int poz, resenje, resenjeR, i, n, brojac = 0;
  char rec[MAX_NIZA];
  char recR[MAX_NIZA];

  scanf("%d", &poz);

  recR[0] = rec[0];
  recR[1] = rec[1];
  recR[2] = rec[2];
  recR[3] = rec[3];
  recR[4] = rec[4];
  recR[5] = rec[5];
  recR[6] = rec[6];
  recR[7] = rec[7];
  recR[8] = rec[8];
  recR[9] = rec[9];

  n = mystrlen(rec);
  resenje = ubaci(rec, poz, n);

  if (n > 20 || n <= poz || poz < 0)
    resenjeR = 0;
  else {
    recR[n + 1] = '\0';
    for (i = n; i >= poz; i--)
      recR[i + 1] = recR[i];
    resenjeR = 1;
  }

  assert(resenje == resenjeR);
  assert(recR[0] == rec[0]);
  assert(recR[1] == rec[1]);
  assert(recR[2] == rec[2]);
  assert(recR[3] == rec[3]);
  assert(recR[4] == rec[4]);
  assert(recR[5] == rec[5]);
  assert(recR[6] == rec[6]);
  assert(recR[7] == rec[7]);
  assert(recR[8] == rec[8]);
  assert(recR[9] == rec[9]);

  return 0;
}
