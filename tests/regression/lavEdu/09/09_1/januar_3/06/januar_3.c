#include <stdio.h>
#include <stdlib.h>

typedef
struct razlomak
	{
	unsigned imenilac;
	unsigned brojilac;
	} RAZLOMAK;

unsigned manji_ili_jednak(RAZLOMAK a, RAZLOMAK b)
	{
	float odnosA, odnosB;

	odnosA = (float)a.imenilac/a.brojilac;
	odnosB = (float)b.imenilac/b.brojilac;
	
	if(odnosA <= odnosB)
		return 1;
	else 
		return 0;
	}

void ucitaj_razlomke(RAZLOMAK razlomci[], unsigned n)
	{
	int i;
	
	for(i=0; i<n; i++)
		scanf("%u %u", &razlomci[i].imenilac, &razlomci[i].brojilac);
	}

void ispisi_razlomke(RAZLOMAK razlomci[], unsigned n)
	{
	int i;

	for(i=0; i<n; i++)
		printf("%d/%d ", razlomci[i].imenilac, razlomci[i].brojilac);
	printf("\n");
	}

RAZLOMAK maksimum (RAZLOMAK* razlomci, unsigned n)
	{
	int i;
	RAZLOMAK max;
	
	max.imenilac = razlomci[0].imenilac;
	max.brojilac = razlomci[0].brojilac;

	for(i=1; i<n; i++)
		if(manji_ili_jednak(max, razlomci[i]))
			max = razlomci[i];

	return max;
	}

int main()
	{
	RAZLOMAK razlomci[2], max;
	unsigned N;
	RAZLOMAK *k;
	
	ucitaj_razlomke(razlomci, 2);
	printf("(b): %d\n", manji_ili_jednak(razlomci[0], razlomci[1]));
	
	scanf("%u", &N);
	k = malloc(N*sizeof(RAZLOMAK));
	if(k == NULL)
		{
		printf("GRESKA!");
		return 0;
		}
	
	ucitaj_razlomke(k, N);
	printf("(d): ");
	ispisi_razlomke(k, N);
	
	max = maksimum(k, N);
	printf("(e): %d/%d\n", max.imenilac, max.brojilac);
	
	return 0;
	}

