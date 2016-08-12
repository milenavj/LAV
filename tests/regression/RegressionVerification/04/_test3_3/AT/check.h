#ifndef SIZE1
#define SIZE1 10
#endif
#ifndef SIZE2
#define SIZE2 10
#endif

int assert(int);

int main() {
  int matrica[SIZE1][SIZE2], n, m, i, j, niz_maksimuma[SIZE1],
      niz_maksimumaR[SIZE1], maxi;

  printf("Unesi broj vrsta: ");
  scanf("%d", &n);

  printf("Unesi broj kolona: ");
  scanf("%d", &m);

  if (n <= 0 || n > SIZE1 || m <= 0 || m > SIZE2) {
    printf("Izlaz:\n-1\n");
    return 0;
  } else {
    printf("Unesi elemente matrice:\n");

    for (i = 0; i < n; i++)
      for (j = 0; j < m; j++)
        scanf("%d", &matrica[i][j]);

    for (i = 0; i < n; i++) {
      maxi = matrica[i][0];
      for (j = 1; j < m; j++)
        if (matrica[i][j] > maxi)
          maxi = matrica[i][j];
      niz_maksimumaR[i] = maxi;
    }

    maksimumi(matrica, n, m, niz_maksimuma);

    for (i = 0; i < n; i++)
      assert(niz_maksimuma[i] == niz_maksimumaR[i]);
  }

  return 0;
}
