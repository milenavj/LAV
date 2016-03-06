#include <stdio.h>
#define DUZINA 100
float max_broj(float niz[],int x);
float aritmeticka_sredina(float niz[],int n);
void unesi(float niz[], int n);
int main(){
	int n;
	float r,max,niz[DUZINA];
	printf("Unesite broj clanova niza: \n");
	scanf("%d",&n);
	if(n<=0) printf("-1\n");
	else{
	unesi(niz,n);
	max=max_broj(niz,n);
	r=aritmeticka_sredina(niz,n);
	printf("%f\n",max);
	printf("%f\n",r);
	if (max>2*r) printf("1\n");
	else printf("0\n");
	}
}

void unesi(float niz[], int n){
	int i;
	for(i=0;i<n;i++){
		scanf("%f",&niz[i]);
	}
}

float max_broj(float niz[], int x){
	int i;
	float max;
	max=niz[0];
	for(i=1;i<x;i++){
		if(max<niz[i]){
			max=niz[i];
		}
	}
	return max;
}
float aritmeticka_sredina(float niz[],int n){
	int i;
	float s=0,r;
	for(i=0;i<n;i++){
		s+=niz[i];
		r=s/n;
	}
	return r; 
}