#include <stdio.h>
#define max 10   /* Definisemo maximalnu dimenziju matrice*/
void zbir(float a[][max],int n,float *sum){/* Funkcija koja ima povratnu vrednost void, a elementi su joj dva realnog tipa i jedan celobrojnog tipa*/
int i,j;
		 *sum=0.0;/* Postavimo sumu na 0*/
for(i=0;i<n-1;i++){ 
	for(j=n-2-i;j>=0;j--){ /* Odredimo koji su elementi iznad  dijagonale*/
			  *sum=*sum+a[i][j]; /* Sumiramo te elemente, pomocu pokazivaca da bi se vrednos sum promenila*/
	}

}
}



int main(){
	int n,i,j; /* Definisemo pomocne promenljive i dimenziju  matrice celobrojnog tipa*/
	float a[max][max],sum; /* Definisemo matricu i promenljivu za sumu elemenata realnog tipa*/
	printf("Uneti ceo broj: ");
	scanf("%d",&n); /* Unosimo dimenziju matrice*/
	if(n>0 && n<=max){ /* Uslov da nam dimenzije matrice ne predju granicu koju smo odredili*/
	for(i=0;i<n;i++){
		for(j=0;j<n;j++){
			scanf("%f",&a[i][j]); /* Unosimo elemente matrice*/
		}}
		zbir(a,n,&sum); /* Pozivamo funkciju za zbir*/
		printf("Izlaz: %f\n",sum); /* Ispisujemo zbir*/
	}
		else
			printf("Izlaz: -1\n"); /* Ispisujemo -1 ako nisu dobro unesene dimenzije matrice*/
return 0;
}
