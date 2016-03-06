#include <stdio.h>
#define MAKS_NIZ 100

float max_broj(float niz[], int n){ /*funkcija sa unos niza argumenti niz i njegova duzina*/
	float max=niz[0];		/*dodelimo maksimumu prvi clan niza*/
	int i;

	for(i=0;i<n;i++)
		if(niz[i]>max)max=niz[i];   /*ukoliko je neki clan niza veci od max, on postaje max*/

	return max;
}

float aritmeticka_sredina(float niz[], int n){ /*funkcija za arit. sr. niza*/
	float suma=0.0;
	int i;

	for(i=0;i<n;i++)suma+=niz[i];		/*odredjujemo sumu clanova niza koja nam je potrebna za ar.s*/
	
	return suma/n;
}


int main(){
	int n,i;
	float niz[MAKS_NIZ], max, arit;		

	printf("Unesite duzinu niza n: ");
	scanf("%d", &n);

	if(n<=0 || n>MAKS_NIZ){ 	/*ukoliko je duzina niza manja ili jednaka 0, greska, ili veca od max 				duzine niza*/
		printf("-1\n");	
		return 0;
                }
	
/*	printf("Unesite %d clanova niza: ", n);*/
	for(i=0;i<n;i++)scanf("%f", &niz[i]);    	/*petlja za unos clanova niza*/
			 
	max=max_broj(niz, n);				/*vrednost funkcije dodeljujemo promenljivama*/
	arit=aritmeticka_sredina(niz, n);
	
	printf("%f\n", max);				/*ispis funkcije max*/
	printf("%f\n", arit);				/*ispis funkcije arit. sr*/

	if(max>2*arit) printf("1\n");			/*ukoliko je max strogo veci pisemo 1 inace 0*/
	else printf("0\n");
return 0;
}
	









