#include<stdio.h>
#define MAX 100

int indeks_minimuma(float a[],int n){
	int i,indeks=0;
	float min=a[0];
	for(i=1;i<n;i++)
		if(a[i]<=min){
			min=a[i];
			indeks=i;
}
	return indeks;
}
int indeks_maksimuma(float a[],int n){
	int i,indeks=0;
	float max=a[0];
	for(i=0;i<n;i++)
		if(a[i]>=max){
			max=a[i];
			indeks=i;
}
return indeks;
}

int main(){
	float a[MAX];
        int n,i;
        printf("Unesite dimenziju niza:\n");
        scanf("%d", &n);
	if(n<=0)
		printf("-1\n");
	else{
	printf("Uneti niz:\n");
        for(i=0;i<n;i++)
               scanf("%f", &a[i]);

	printf("Izlaz:\n");	
	printf("%d\n", indeks_minimuma(a,n));
	printf("%d\n", indeks_maksimuma(a,n));
	if(indeks_minimuma(a,n)>indeks_maksimuma(a,n))	
		printf("1\n");
	else
		printf("0\n");
}
}	

