#include <stdio.h>

int min(int i, int j)
{
  return (i<j) ? i : j;
}

int main()
{
  int n, i, j;
  scanf("%d", &n);
  if(n < 0 || (n&1) == 0) //n&1 radi brze nego n%2, a radi istu stvar (parnost)
    {
      printf("*\n"); //jer to je neispravan unos!
      return 1; //exit(1);
    }
  for(i=0; i<n; i++)
    {
      int m = min(i, n-1-i); //jer treba da bude "simetricno" po horizontali
      //kad se predje pola, broj tacaka se smanjuje i zato uzimamo minimum
      for(j=0; j<m; j++) //ispisujemo m tacaka
	printf(".");
      for(; j<n-m; j++) //zatim x-ove, osim poslednjih m (zato ide do n-m)
	printf("x");
      for(; j<n; j++) //pocetna vrednost je n-m, dakle bice m tacaka ispisano
	printf(".");
      printf("\n"); //kraj jednog reda
    }
  return 0;
}
