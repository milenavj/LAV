#include <stdio.h>
#include <stdlib.h>

typedef struct RAZLOMAK1
{
	unsigned broj;
	unsigned imen;
} RAZLOMAK;

unsigned manji_ili_jednak(RAZLOMAK a, RAZLOMAK b)
{
	float kol1, kol2;
	kol1 = (float)a.broj/a.imen;
	kol2 = (float)b.broj/b.imen;
	if(kol1<=kol2)
		return 1;
	else
		return 0;
}

void ucitaj_razlomke(RAZLOMAK razlomci[], unsigned n)
{
	int i;
	for(i=0; i<n; i++)
		scanf("%u%u", &(razlomci[i].broj), &(razlomci[i].imen));
}

void ispisi_razlomke(RAZLOMAK razlomci[], unsigned n)
{
	int i;
	for(i=0; i<n; i++)
		printf("%u/%u ", razlomci[i].broj, razlomci[i].imen);
	printf("\n");
}

RAZLOMAK maksimum(RAZLOMAK *razlomci, unsigned n)
{
	int i;
	RAZLOMAK max;
	max = razlomci[0];
	for(i=0; i<n; i++)
		{
			if(manji_ili_jednak(max, razlomci[i]) == 1)
				max = razlomci[i];
		}
	return max;
}

int main()
{
	RAZLOMAK a, b, c;
	RAZLOMAK *niz;
	int i;
	unsigned N;
	scanf("%u%u", &(a.broj), &(a.imen));
	scanf("%u%u", &(b.broj), &(b.imen));
	
	i = manji_ili_jednak(a, b);
printf("(b): ");	
	if(i == 1)
		printf("1\n");
	else
		printf("0\n");
	
	scanf("%u", &N);
	niz = (RAZLOMAK*)malloc(N*sizeof(RAZLOMAK));
	if(niz == NULL)
		{
			printf("GRESKA!\n");
			return -1;
		}
	ucitaj_razlomke(niz, N);
printf("(d): ");	
	ispisi_razlomke(niz, N);
	
printf("(e): ")	;
	c = maksimum(niz, N);
	printf("%u/%u\n", c.broj, c.imen);
	
	
	
	free(niz);
	niz = NULL;
	
	return 0;
}
