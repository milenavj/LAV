#include <stdio.h>
#define MAX_SIZE 100 /*definisemo maksimalan broj elemenata niza*/

#if 0
void ucitaj(float a[], int n) /*funkcija za upis niza*/
    {
  int i;
  for (i = 0; i < n; i++)
    scanf("%f", &a[i]);
}
#endif

int indeks_minimuma(float a[], int n) {
  float min;
  int k = 0, i;
  min = a[0]; /*postavimo da je prvi clan minimum*/
  /*u ovoj for petlji ispitujemo da li je neki od clanova niza 	
    manji od minimuma, ako jeste, njega postavimo za
    minimum, i pamtimo njegov indeks*/
  for (i = 1; i < n; i++)
    if (a[i] <= min) {
      min = a[i];
      k = i;
    }
  return k; /*funkcija vraca indeks*/
}
int indeks_maksimuma(float a[], int n) {
  float max;
  int k = 0, i;
  max = a[0]; /*prvi clan niza postavimo za maksimum*/
  /*ispitujemo da li je neki od preostalih clanova veci od opstavljenog
    maksimuma, i ako jeste,
    njega postavimo za maksimum, i pamtimo njegov indeks*/
  for (i = 1; i < n; i++)
    if (a[i] >= max) {
      max = a[i];
      k = i;
    }
  return k;
}

int main() {
  float a[MAX_SIZE];
  int n;
  printf("Unesite dimenziju niza:\n");
  scanf("%d", &n);

  /*za pogresan unos*/
  if (n <= 0 || n > MAX_SIZE) {
    printf("-1\n");
    return 0;
  }
  printf("Uneti niz:  ");
  ucitaj(a, n);
  /*ispisujemo indekse*/
  printf("Izlaz:\n%d\n%d\n", indeks_minimuma(a, n), indeks_maksimuma(a, n));
  /*ispisujemo 1, ako je uslov zadovoljen*/
  (indeks_minimuma(a, n) > indeks_maksimuma(a, n)) ? printf("1\n")
                                                   : printf("0\n");

  return 0;
}
