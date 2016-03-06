#include <stdio.h>
#include <math.h>
#define DIMENZIJA 100
int indeks_minimuma (float niz[], int dimenzija);
int indeks_maksimuma (float niz[], int dimenzija);
int unos_niza (float niz[]); // funkcija za unos niza brojeva tipa float

int main()
{
  float niz[DIMENZIJA]; // najveci broj elemenata je 100 i definisan je preko #define
  int dimenzija, i, indeksMinimuma, indeksMaksimuma, izlaz; // za promenljive koristim kamilju notaciju, 
  
  dimenzija = unos_niza (niz);
  if (dimenzija == -1)// program provera da li je dimenzija pravilno uneta, ako nije funkcija main vraca 1 zbog neuspesnosti izvrsavanja zadatka i program se zavrsava.
  {
   printf("Izlaz:\n%i\n", dimenzija); //ispisuje -1 ako nije pravilno uneta dimenzija 
   return 1;
  }
  
  indeksMinimuma = indeks_minimuma (niz, dimenzija); // promenljive u koje smestam izlazne vrednosti funkcija indeks_minimuma i indeks_maksimuma
  indeksMaksimuma = indeks_maksimuma (niz, dimenzija);
  
  if (indeksMinimuma > indeksMaksimuma) // uporedjivanje vrednosti indeksa minimuma i maksimuma
    izlaz = 1;
  else
    izlaz = 0;
  
  printf("%i\n%i\n%i\n", indeksMinimuma, indeksMaksimuma, izlaz);
  
  return 0;// program se uspesno izvrsava
  
}

int indeks_minimuma (float niz[], int dimenzija)
{
  int i, indeks;
  float minimum;

  for (i = 1, minimum = niz[0], indeks = 0 ; i < dimenzija; i++) // minimumu dodeljujem vrednost prvog elementa niza
  
    if (minimum >= niz[i])// vrednost minimuma se uporedjuje sa svakim elementom pocev od elementa sa indeksom 1
    {			  // a zatim ako je element veci ili jednak od minimuma, minimum dobija vrednost tog elementa
      minimum = niz[i];   // a indeks minimuma postaje indeks tog elementa
      indeks = i;
    }
    
    return indeks;// funkcija vraca indeks
}

int indeks_maksimuma (float niz[], int dimenzija)// slicno kao i kod funkcije indeks_minimuma, jedino sto se proverava
{						// da li je element  veci ili jednak maksimum
  int i, indeks;
  float maksimum;

  for (i = 1, maksimum = niz[0], indeks = 0 ; i < dimenzija; i++)
    if (maksimum <= niz[i])
    {
      maksimum = niz[i];
      indeks = i;
    }
    
    return indeks;
}

int unos_niza (float niz[]) // funkcija pomocu koje se unosi niz
{
 int i, dimenzija;
 
 printf("Uneti dimenziju niza:\t");
 scanf("%i", &dimenzija);
 
 
 
 if (dimenzija < 1 || dimenzija > DIMENZIJA) // proverava se ispravnost dimenzije niza, ako nije ispravna, niz vraca dimenziju niza -1
   return -1;
 
 printf("Uneti niz:\t"); // ako je dimenzija u redu, funkcija nastavlja izvrsavanje
 
 for (i = 0; i < dimenzija; i++)
   scanf("%f", & niz[i]);
 
 return dimenzija;
 
}