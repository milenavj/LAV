#include <stdio.h>
#define maxdim 100

float indeks_minimuma(float a[],int n);
float indeks_maksimuma(float a[],int n);

int main(){
	float a[maxdim];
	int n,i;
	printf("Unesite dimenziju niza:\n");
	scanf("%d",&n);
	/* ako je dimenzija manja od 0 ili veca od maksimalne nekorektan unos*/
	if(n<=0 || n>maxdim){
		printf("Izlaz: \n -1\n");
		return 0;
	}
	else {
		printf("Uneti niz:\n");
		for(i=0;i<n;i++){
			scanf("%f",&a[i]);
		}
		printf("Izlaz:\n");
		printf( "%f\n",indeks_minimuma(a,n));
		printf("%f\n",indeks_maksimuma(a,n));
		if(indeks_minimuma(a,n) > indeks_maksimuma(a,n))
			printf("1\n");
		else
			printf("0\n");
	}
	return 0;
		
}

float indeks_minimuma(float a[],int n){
	int i,j;
	float min;
	min=a[0];
	j=0;
	/*prvi je ne pocetku minimalni,ako je neki sledeci manji ili jednak njemu on postaje minimalni*/
	for(i=0;i<n;i++){
		if(a[i]<=min){
			min=a[i];
			j=i;
		}
			
	}	
	/*cuvam indekse u promenljivoj j i to vracam*/
	return j;
}

float indeks_maksimuma(float a[],int n){
	int i,j;
	float max;
	max=a[0];
	j=0;
/*prvi je ne pocetku maksimalni,ako je neki sledeci veci ili jednak njemu on postaje maksimalni*/
	for(i=0;i<n;i++){
		if(a[i]>=max){
			max=a[i];
			j=i;
		}

	}
	/*cuvam indekse u promenljivoj j i to vracam*/
	return j;
}

