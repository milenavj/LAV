#include <stdio.h>
#include <stdlib.h>

// a)
typedef struct razlomak
{
  unsigned x, y; //jer je u zadatku receno da su pozitivni razlomci
} RAZLOMAK;

// b)
unsigned manji_ili_jednak(RAZLOMAK a, RAZLOMAK b)
{
  //imamo dva razlomka, a/b i c/d.
  //uslov a/b < c/d je isto sto i a*d < b*c
  return (a.x * b.y <= a.y * b.x);
}

// c)
void ucitaj_razlomke(RAZLOMAK razlomci[], unsigned n)
{
  int i;
  for(i=0; i<n; i++)
    {
      scanf("%u%u", &(razlomci[i].x), &(razlomci[i].y));
    }
}

// d)
void ispisi_razlomke(RAZLOMAK razlomci[], unsigned n)
{
  int i;
  for(i=0; i<n; i++)
    {
      printf("%u/%u ", razlomci[i].x, razlomci[i].y);
      //ispis oblika brojilac/imenilac
    }
}

// e)
RAZLOMAK maksimum(RAZLOMAK* razlomci, unsigned n)
{
  int i;
  RAZLOMAK izlaz; // = razlomci[0];
  
  //negde moze da se dodeljuje cela struktura, negde ne. Ovaj nacin je siguran.
  izlaz.x = razlomci[0].x;
  izlaz.y = razlomci[0].y;
  for(i=1; i<n; i++) //proveravamo od drugog unetog, jer je prvi najveci
    if(manji_ili_jednak(izlaz, razlomci[i])) //odnosno, razlomci[i] >= izlaz
      {
	izlaz.x = razlomci[i].x; //postavi novi najveci
	izlaz.y = razlomci[i].y;
      }
  
  return izlaz;
}

// f)
int main()
{
  RAZLOMAK r1, r2, *razlomci, rmax;
  unsigned N, i;
  
  ucitaj_razlomke(&r1, 1); //unosi samo jedan razlomak
  //jer svejedno je da li prosledili niz ili pokazivac (na prvi element)
  ucitaj_razlomke(&r2, 1);
  printf("(b): %u\n", manji_ili_jednak(r1, r2)); //ispis 0 ili 1
  
  scanf("%u", &N);
  razlomci = (RAZLOMAK*)malloc(N * sizeof(RAZLOMAK)); //dinamicka alokacija
  if(razlomci == NULL) //nije uspela dinamicka alokacija
    {
      printf ("GRESKA!\n"); //ispisi poruku o gresci
      return 1; //i zavrsi s radom odmah
    }
  ucitaj_razlomke(razlomci, N);
  printf("(d): ");
  ispisi_razlomke(razlomci, N);
  printf("\n");
  
  rmax = maksimum(razlomci, N); //poziv f-je
  printf("(e): ");
  ispisi_razlomke(&rmax, 1); //na istu foru kao unos jednog razlomka
  //dakle pokazivac na (jedini) element niza, je f-ji isto sto i niz
  printf("\n");
  
  //ONE LAST NOTE: nije moralo da se koristi rmax
  //moglo je i ispisi_razlomke(&maksimum(razlomci, N), 1);
  return 0;
}
