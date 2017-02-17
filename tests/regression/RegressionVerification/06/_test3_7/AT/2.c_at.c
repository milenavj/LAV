
#include <stdio.h>
#define MAX 11

void zbir(float a[][MAX], int n, float *sum);

#if 0
int main() {

  float a[MAX][MAX];
  int i, j;  //brojaci
  int n;     //dimenzija
  float sum; //suma

  sum = 0;

  printf("Unesi ceo broj : \n");
  scanf("%d", &n);

  if (n > 10 || n <= 0)
    printf("Izlaz: -1\n"); //uslov za dimenziju,i -1 ako nije dobra dimenzija
  else {
    printf("Uneti matricu : \n");
    for (i = 1; i <= n; i++)
      for (j = 1; j <= n; j++)
        scanf("%f", &a[i][j]);

    zbir(a, n, &sum); //kao argument se navodi ime matrice,dimenzija i
                      //promenljiva za rezultat (pokazivackog tipa)

    printf("Izlaz : %f \n", sum);
  }

  return 0;
}
#endif

void zbir(float a[][MAX], int n, float *sum) {

  int i, j; //brojaci

  for (i = 1; i <= n; i++)
    for (j = 1; j <= n; j++)
      if ((i + j) < n + 1)
        *sum += a[i][j];

}
#include "check.h"
