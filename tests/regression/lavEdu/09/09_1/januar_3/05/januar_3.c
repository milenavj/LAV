#include <stdio.h>
#include <stdlib.h>

typedef struct{

	unsigned imenilac;
	unsigned brojilac;
	
}RAZLOMAK;

unsigned manji_ili_jednak(RAZLOMAK a, RAZLOMAK b){
	float prvi, drugi;
	prvi = (float)a.imenilac/ (float)a.brojilac;
	drugi = (float)b.imenilac/ (float)b.brojilac;

	if(prvi<=drugi) return 1;
	else return 0;

}

void ucitaj_razlomke(RAZLOMAK razlomci[], unsigned n){
	int i;

	for(i=0;i<n;i++){
		scanf("%u", &razlomci[i].imenilac);
		scanf("%u", &razlomci[i].brojilac);
	}

}

void ispisi_razlomke(RAZLOMAK razlomci[], unsigned n){
	int i;
	for(i=0;i<n;i++){
		printf("%u/%u ", razlomci[i].imenilac, razlomci[i].brojilac);

}

}

RAZLOMAK maksimum(RAZLOMAK* razlomci, unsigned n){
	int i;
	RAZLOMAK MAKS = razlomci[0];
	for(i=0;i<n;i++){
		if(manji_ili_jednak(MAKS, razlomci[i]))MAKS = razlomci[i];
		}
	return MAKS;
}


int main(){
	RAZLOMAK a, b, maks;
	unsigned n;
	RAZLOMAK *p;

	scanf("%u%u%u%u", &a.imenilac, &a.brojilac, &b.imenilac, &b.brojilac);
	if(manji_ili_jednak(a,b)) printf("(b): 1\n");
	else printf("(b): 0\n");
	
	scanf("%u", &n);
	p = (RAZLOMAK*)malloc(n*sizeof(RAZLOMAK));
	if(p == NULL){
		printf("GRESKA!\n");
		return 0;
		}

	ucitaj_razlomke(p, n);
	printf("(d): ");
	ispisi_razlomke(p, n);
	printf("\n");

	maks=maksimum(p, n);
	printf("(e): %u/%u\n", maks.imenilac, maks.brojilac);

return 0;


} 



