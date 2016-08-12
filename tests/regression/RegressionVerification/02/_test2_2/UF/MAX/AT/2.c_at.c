int __UF__4_1(float *, int, int, int);
int __UF__4_0(float *, int, int, int);

#define __RET_UNDEF -1
#include <stdio.h>
#define MAXD 100

int indeks_maksimuma(float a[], int n) {
  int max = 0; //max = n-1 (alternativni metod - idi od kraja niza)
  int i;
  i = 1;
  if (i < n) {
    if (a[i] >= a[max]) //i kad je jednak trenutnom maksimumu
      max = i;
    i++;
    max = __UF__4_0(a, n, i, max);
  }
  ; //treba da postane novi maksimum
  return max; //jer se gleda POSLEDNJE pojavljivanje, a petlja ide od pocetka
              //niza
    //(ako bi isao od kraja niza onda gore u if-u ne bi bio operator >= nego >)
}

int indeks_minimuma(float a[], int n) {
  int min = 0;
  int i;
  i = 1;
  if (i < n) {
    if (a[i] <= a[min]) //i ovde vazi isto pravilo kao za indeks_maksimuma
      min = i;
    i++;
    min = __UF__4_1(a, n, i, min);
  }
  ; //jer potreban je indeks poslednjeg pojavljivanja!
  return min;
}

#if 0
int main() {
  int n, i, min, max;
  float niz[MAXD];

  printf("Uneti dimenziju niza: ");
  scanf("%d", &n);
  if (n <= 0 || n > MAXD) { //provera ispravnosti dimenzije
    printf("-1\n");
    return 0; //izlaz odmah, jer je neispravan
  }

  printf("Uneti niz: ");
  for (i = 0; i < n; i++)
    scanf("%f", &niz[i]); //unosenje jednog po jednog elementa - moglo je pisati
                          //i niz+i umesto &niz[i]

  //poziv funkcija za indeks minimuma i maksimuma
  min = indeks_minimuma(niz, n);
  max = indeks_maksimuma(niz, n);

  printf("%d\n%d\n", min, max); //ispisuje prvo index minimuma, pa maksimuma
  if (min > max) //provera da li je indeks minimuma strogo veci od ovog drugog
    printf("1\n");
  else
    printf("0\n");

  return 0;
}
#endif
#include "../check.h"
