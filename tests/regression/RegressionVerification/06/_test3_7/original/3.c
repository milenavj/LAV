/*Predprocesorske direktive, prva za ukljucivanje funkcija iz stdio.h standardne
 * biblioteke i druga za oznacavanje maksimalne dimenzije niza*/
#include <stdio.h>
#define MAX 10
void zbir(float a[][MAX], int n,
          float *sum) //suma za zbir, sa argumentima navedenim u zadatku.
    {
  int i, j;
  float s = 0;
  for (i = 0; i < n; i++)
    for (j = n - 2 - i; j >= 0; j--)
      *sum += a[i][j];
}
int main() {
  float a[MAX][MAX], sum = 0; //deklarisanje dvodimenzionalnog niza
  int n, i, j;
  scanf("%d", &n); //unos dimenzije niza
  if (n <= MAX && n > 0) // proverava se da li je dimenzija pozitivan broj ne
                         // veci od maksimalne dimenzije
      {
    for (i = 0; i < n; i++)
      for (j = 0; j < n; j++)
        scanf("%f", &a[i][j]);  //Unos elemenata niza
    zbir(a, n, &sum);           //Poziv funkcije zbir
    printf("sum= %.3f\n", sum); //stampanje date sume
  } else
    printf("-1\n"); //Ukoliko je dimenzija negativan broj ili veca od maksimalne
                    //ispisuje se -1
  return 0;
}
