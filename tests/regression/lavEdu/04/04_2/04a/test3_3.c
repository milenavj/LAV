#include<stdio.h>
#define max_size 10

void maksimumi(int a[][max_size], int v, int k, int b[]){
	int i, j, maks=a[0][0]; /*maks- trenutni maksimum(resetuje se pre prelaska u novu vrstu)*/
	for(i=0; i<v; i++){
		for(j=0; j<k; j++)
			if(maks < a[i][j])
				maks = a[i][j];

	b[i] = maks;
	maks=a[i+1][0];	
	}
	return;
}




int main(){
	int a[max_size][max_size], b[max_size], v, k, i, j; /*v- broj vrsta, k-broj kolona, i,j- brojaci*/
	
	printf("Unesi broj vrsta matrice, i broj kolona matrice:\n");
	scanf("%d%d", &v, &k);

	if( v <= 0 || v > 10 || k <= 0 || k > 10 ){ /*provera uslova*/
		printf("-1");
		return 0;
	}

	/*unos*/
	printf("Unesi elemente matrice:\n");
	for(i=0; i<v; i++)
		for(j=0; j<k; j++)
			scanf("%d", &a[i][j]);
	

	maksimumi(a, v, k, b);
	
	/*ispis*/
	for(i=0; i<v; i++)
		printf("%d\n", b[i]);
	
	return 0;
}

