#include <stdio.h>
#include <stdlib.h>

typedef struct razlomak {
	
	int brojilac;
	int imenilac;
} RAZLOMAK ;

unsigned manji_ili_jednak(RAZLOMAK a, RAZLOMAK b){
	
	if(a.brojilac * b.imenilac <= b.brojilac * a.imenilac)
		return 1;
	else 
		return 0;
	

}


void ucitaj_razlomke(RAZLOMAK razlomci[], unsigned n){

	int i;
	unsigned a, b;
	
	
	for(i = 0; i < n; i++){

		scanf("%u%u", &a, &b);
		
		razlomci[i].brojilac = a;
		razlomci[i].imenilac = b;
	}


}

void ispisi_razlomke(RAZLOMAK razlomci[], unsigned n){
	
	int i;

	for(i = 0; i < n; i++){
		printf("%d/%d ", razlomci[i].brojilac, razlomci[i].imenilac);
	}

}

RAZLOMAK maksimum(RAZLOMAK razlomci[], unsigned n){
	
	RAZLOMAK max;
	int i;

	max = razlomci[0];

	for(i = 1; i < n; i++){
		if(max.brojilac * razlomci[i].imenilac < razlomci[i].brojilac * max.imenilac)
			max = razlomci[i];

	}

	return max;

}

int main(){
	RAZLOMAK a, b, max;
	unsigned N;

	scanf("%d%d", &a.brojilac, &a.imenilac);
	
	scanf("%d%d", &b.brojilac, &b.imenilac);

	RAZLOMAK *p;

	if(manji_ili_jednak(a, b))
		printf("(b): 1");
	else
		printf("(b): 0");

	printf("\n");

	scanf("%u", &N);

	p = (RAZLOMAK*)malloc(N * sizeof(RAZLOMAK));

	if(p == NULL){
		printf("GRESKA!\n");
		return 0;
	}


	ucitaj_razlomke(p, N);

	printf("(d): ");

	ispisi_razlomke(p, N);

	printf("\n");

	max = maksimum(p, N);

	printf("(e): %d/%d\n", max.brojilac, max.imenilac);
	

	free(p);
	p = NULL;

	return 0;
} 
