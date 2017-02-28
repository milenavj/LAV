#include<stdio.h>
#include<stdlib.h>

typedef struct razlomak /*definise se tip podataka RAZLOMAK*/
{
    int imenilac;
    int brojilac;
} RAZLOMAK;

unsigned manji_ili_jednak(RAZLOMAK a, RAZLOMAK b) /*funkcija za poredjenje 2 razlomka*/
{
  if(a.imenilac*b.brojilac <= b.imenilac*a.brojilac) return 1; 
  else return 0;
  
}

void ucitaj_razlomke(RAZLOMAK razlomci[], unsigned n) /*funkcija za ucitavanje niza razlomaka*/
{
  int i;
  
  for(i=0; i<n; i++) /*N puta se ucitava imenilac i brojilac razlomka*/
  {
    printf("Unesi imenilac\n");
    scanf("%d", &razlomci[i].imenilac);
    
    printf("Unesi brojilac\n");
    scanf("%d", &razlomci[i].brojilac);
    
  }
  
}


void ispisi_razlomke(RAZLOMAK razlomci[], unsigned n)/*funkcija za ispisivanje razlomaka*/
{
  int i;
  
  for(i=0; i<n; i++) 
  {
    printf("%d/%d ", razlomci[i].imenilac, razlomci[i].brojilac); /*ispisuju se svaki razlomak iz niza razlomci*/
  }
  
}

RAZLOMAK maksimum(RAZLOMAK* razlomci, unsigned n) /*funkcija za razunanje maksimuma, prvi razlomak se postavlja na maksimum i porede se ostali razlomci*/
{
  RAZLOMAK max;
  max.imenilac = razlomci[0].imenilac;
  max.brojilac = razlomci[0].brojilac;
  
  int i;
  
  for(i=1; i<n; i++)
  {
      if(razlomci[i].imenilac*max.brojilac>max.imenilac*razlomci[i].brojilac)
      {
	 max.imenilac = razlomci[i].imenilac;
	 max.brojilac = razlomci[i].brojilac;
      }
  }
  
return max;  
}


int main()
{
  RAZLOMAK a, b;
  int N;
  RAZLOMAK* razlomci;
  
  printf("Unesi imenilac prvog razlomka\n");
  scanf("%d", &a.imenilac);
  
  printf("Unesi brojilac prvog razlomka\n");
  scanf("%d", &a.brojilac);
  
  printf("Unesi imenilac prvog razlomka\n");
  scanf("%d", &b.imenilac);
  
  printf("Unesi imenilac prvog razlomka\n");
  scanf("%d", &b.brojilac);
  
  printf("(b): %d\n", manji_ili_jednak(a, b));/*deo od pocetka funkcije main do ove linije koristi se za unos 2 razlomka i njihovo poredjenje*/
  
  printf("Unesi n\n");/*unosi se N*/
  scanf("%d", &N);
  
  razlomci = (RAZLOMAK*) malloc(N*sizeof(RAZLOMAK));/*radi se alociranje memorije i prekid programa ako je neuspesno*/
  
  if(razlomci == NULL) 
  {
    printf("GRESKA!\n");
    return 0;
  }
    /*kod ispod poziva funkcije i stampa kako se trazi u delu zadatka pod f*/
  ucitaj_razlomke(razlomci, N);
  
  printf("(d): ");
  ispisi_razlomke(razlomci, N);
  
  printf("\n");
  
  printf("(e): %d/%d\n", maksimum(razlomci, N).imenilac, maksimum(razlomci, N).brojilac);
  
return 0;
}
