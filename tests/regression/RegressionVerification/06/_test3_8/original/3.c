#include <stdio.h>
#define MAX 10

//prototip funkcije zbir
void zbir(float a[][MAX], int n, float *sum);
int main() {

  int n, i, j;
  float sum = 0;
  float a[MAX][MAX];

  //unosimo dimenziju matrice, proveravamo da li je dimenzija korektna
  printf("Uneti ceo broj: ");
  scanf("%d", &n);
  if (n > MAX || n <= 0) {
    printf("-1");
    return 0;
  }

  //unosimo nxn matricu
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      scanf("%f", &a[i][j]);

  zbir(a, n, &sum);
  printf("%f", sum);

}
//definicija funkcije zbir koja racuna zbir elemenata ispod sporedne dijagonale
void zbir(float a[][MAX], int n, float *sum) {

  int i, j;
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      if (j >= (n - i))
        *sum += a[i][j];

}
