#include<stdio.h>
#include<stdlib.h>

typedef struct razlomak
{
unsigned brojilac,imenilac;
}RAZLOMAK;

void ucitaj_razlomke(RAZLOMAK razlomci[],unsigned n)
{
int i;
for(i=0;i<n;i++)
	{
	scanf("%u",&razlomci[i].brojilac);
	scanf("%u",&razlomci[i].imenilac);
	}
}


void ispisi_razlomke(RAZLOMAK razlomci[],unsigned n)
{
int i;
for(i=0;i<n;i++)
	{
	printf("%u/",razlomci[i].brojilac);
	printf("%u ",razlomci[i].imenilac);
	
	}
}
unsigned manji_ili_jednak(RAZLOMAK a,RAZLOMAK b)
{
if((double)a.brojilac/a.imenilac<=(double)b.brojilac/b.imenilac)
		return 1;
return 0;
}

RAZLOMAK maksimum(RAZLOMAK* razlomci,unsigned n)
{
int i;
RAZLOMAK max;
max=razlomci[0];
for(i=1;i<n;i++)
	if(manji_ili_jednak(max,razlomci[i]))
		max=razlomci[i];
return max;
}




int main()
{

unsigned N;

RAZLOMAK a,b,*razlomci;

scanf("%u %u %u %u",&a.brojilac,&a.imenilac,&b.brojilac,&b.imenilac);
printf("(b): %u\n",manji_ili_jednak(a,b));

scanf("%u",&N);/*Unos broja razlomaka*/

razlomci=malloc(N*sizeof(RAZLOMAK));
if(razlomci==NULL)
	{
	printf("GRESKA!\n");
	return -1;
	}
ucitaj_razlomke(razlomci,N);

printf("(d): ");
ispisi_razlomke(razlomci,N);
printf("\n(e): ");

printf("%u/%u\n",maksimum(razlomci,N));
free(razlomci);
razlomci=NULL;
return 0;
}




