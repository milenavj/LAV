#include<stdio.h>
#include<stdlib.h>

typedef struct razlomak{
	unsigned br;
	unsigned im;
}RAZLOMAK;



unsigned manji_ili_jednak(RAZLOMAK a, RAZLOMAK b){
	if((double)a.br / a.im <= (double)b.br / b.im)
		return 1;
	else return 0;
}


void ucitaj_razlomke(RAZLOMAK razlomci[], unsigned n){
	unsigned i;
	for(i=0; i<n; i++)
		scanf("%u%u", &razlomci[i].br, &razlomci[i].im);
}

void ispisi_razlomke(RAZLOMAK razlomci[], unsigned n){
	unsigned i;
	for(i=0; i<n; i++)
		printf("%u/%u ", razlomci[i].br, razlomci[i].im);
}

RAZLOMAK maksimum(RAZLOMAK* razlomci, unsigned n){	
	RAZLOMAK a;
	unsigned i, j=0;
	double max = (double)razlomci[0].br / razlomci[0].im;
	
	for(i=0; i<n; i++)
		if(max < (double)razlomci[i].br / razlomci[i].im){
			max = (double)razlomci[i].br / razlomci[i].im;
			j=i;
		}
	a.br = razlomci[j].br;
	a.im = razlomci[j].im;

	

	return a;
}



int main(){
	RAZLOMAK a, b, *razlomci, c;
	unsigned N;
	

	scanf("%u%u%u%u", &a.br, &a.im, &b.br, &b.im);

	printf("(b): %u\n", manji_ili_jednak(a, b));

	scanf("%u", &N);

	razlomci = (RAZLOMAK*)malloc(N*sizeof(RAZLOMAK));
	if(razlomci == NULL){
		printf("GRESKA!");
		return 0;
	}
	

	ucitaj_razlomke(razlomci, N);
	
	printf("(d): ");
	ispisi_razlomke(razlomci, N);
	printf("\n");
	
	c = maksimum(razlomci, N);
	printf("(e): %u/%u\n", c.br, c.im);
	
	free(razlomci);
	razlomci = NULL;

	return 0;
}















