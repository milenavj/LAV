#include<stdio.h>
#define SIZE 10 /*definisemo simbolicku konstantu koja ce oznacavati maksimalan broj vrsta i kolona matrice*/

int maksimumi(int matrica[][SIZE], int n, int m, int niz[]) { /*funkcija maksimum prima kao argumente matricu, broj njenih vrsta i kolona, kao i niz u koji treba smestiti rezultat, a kao povratnu vrednost vraca broj clanova dobijenog niza*/
	int i, j, max; 
	for(i=0;i<n;i++) { 
		max = matrica[i][0]; /*postavljamo maksimalnu vrednost vrste na njen prvi element*/
			for(j=1;j<m;j++)
				if(matrica[i][j]>max)	/*proveravamo za svaki element iz vrste da li je veci od maksimuma, i ako jeste, maksimum postavljamo na tu vrednost*/
					max=matrica[i][j];
	niz[i]=max;	/*na kraju prolaska kroz svaku vrstu, dodajemo njen maksimum u niz*/
	}
return i;
}

int main() {
	int niz[50], matrica[SIZE][SIZE], n, m, i, j, k;
	printf("Unesi broj vrsta: ");
	scanf("%d", &n);	/*ucitavanje broja vrsta*/
	printf("Unesi broj kolona: ");
	scanf("%d", &m);	/*ucitavanje broja kolona*/
	if(n<=0 || m<=0 || n>=SIZE || m>=SIZE) {	/*provera korektnosti unosa*/
		printf("Izlaz:\n-1\n");
		return -1;
	}
	printf("Unesi elemente matrice:\n");
	for(i=0;i<n;i++)	/*unos elemenata matrice*/
		for(j=0;j<m;j++)
			scanf("%d", &matrica[i][j]);
	k = maksimumi(matrica, n, m, niz);	/*pozivanje funkcije, i dodela odgovarajuce vrednosti brojacu koji oznacava broj clanova niza*/
	printf("Izlaz:\n");
	for(i=0;i<k;i++)	/*ispis rezultata*/
		printf("%d ", niz[i]);
	printf("\n");
return 0;
}
