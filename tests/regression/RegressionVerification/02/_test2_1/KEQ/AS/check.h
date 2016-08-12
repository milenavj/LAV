#include <stdio.h>
#ifndef MAX_NIZ
#define MAX_NIZ 100
#endif

int assert(int);
int assume(int);

float aritmeticka_sredina(float a[], int n);

float aritmeticka_sredinaR(float a[], int n) {
  int i;
  float suma = 0.0;
  for (i = 0; i < n; i++)
    suma = suma + a[i];

  return suma / (float) n;
}

int main() {

  float a[MAX_NIZ];
  int n;
  float arit1, arit2;

  assume(n > 0 && n <= 100);

  arit1 = aritmeticka_sredinaR(a, n);
  arit2 = aritmeticka_sredina(a, n);
  assert(arit1 == arit2);

  return 0;
}
