#include <stdio.h>

int main() {
  float a11, a12, a21, a22, d;

  scanf("%f", &a11);

  scanf("%f", &a12);

  scanf("%f", &a21);

  scanf("%f", &a22);

  printf("Rezultat dobijene matrice je:\n %.4f \n",
         d = ((a11 * a22) - (a21 * a12)));
  return 0;

}
