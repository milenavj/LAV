#include<stdio.h>
#include<stdlib.h>

typedef struct razlomak {
	int brojilac;
	int imenilac;
}RAZLOMAK;

unsigned manji_ili_jednaki(RAZLOMAK a, RAZLOMAK b) {
	int c = a.imenilac;
	a.imenilac = a.imenilac * b.imenilac;
	a.brojilac = b.imenilac * a.brojilac;
	b.imenilac = a.imenilac;
	b.brojilac = c * b.brojilac;
	if(a.brojilac <= b.brojilac) return 1;
	else return 0;
}
	
void ucitaj_razlomke(RAZLOMAK razlomci[], unsigned n) {
	int i;
	for(i=0;i<n;i++)
		scanf("%d%d", &razlomci[i].brojilac, &razlomci[i].imenilac);
}

void ispisi_razlomke(RAZLOMAK razlomci[], unsigned n) {
	int i;
	for(i=0;i<n;i++)
		printf("%d/%d ", razlomci[i].brojilac, razlomci[i].imenilac);
}

RAZLOMAK maksimum(RAZLOMAK* razlomci, unsigned n) {
	RAZLOMAK max;
	int i;
	max.brojilac = razlomci[0].brojilac;
	max.imenilac = razlomci[0].imenilac;
	for(i=1;i<n;i++)
		if(manji_ili_jednaki(razlomci[i], max)==0) 
			max = razlomci[i];
return max;
}
	

int main() {
	RAZLOMAK razlomak1, razlomak2, max;
	unsigned n;
	RAZLOMAK *niz;
	scanf("%d%d", &razlomak1.brojilac, &razlomak1.imenilac);
	scanf("%d%d", &razlomak2.brojilac, &razlomak2.imenilac);
	if(manji_ili_jednaki(razlomak1, razlomak2)) 
		printf("(b): 1\n");
	else 
		printf("(b): 0\n");
	scanf("%u", &n);
	niz = malloc(n*sizeof(RAZLOMAK));
	if(niz==NULL) {
		printf("GRESKA!");
		return -1;
	}
	ucitaj_razlomke(niz, n);
	printf("(d): ");
	ispisi_razlomke(niz, n);
	max = maksimum(niz, n);
	printf("\n(e): %d/%d\n", max.brojilac, max.imenilac);
	free(niz);
	niz=NULL;
return 0;
}
