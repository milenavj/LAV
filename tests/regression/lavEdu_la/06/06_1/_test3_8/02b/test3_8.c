#include <stdio.h>
#define max 10
void zbir (float a[][max], int n, float *sum) {
int i,j; 
*sum=0; // i ovde *sum=0, ali sam sumu inicijalizovao na 0 u main-u, pre poziva funkcije zbir
for (j=1; j<n; j++)
	for(i=n-j; i<n; i++)
		*sum=*sum+a[i][j];

}

int main () {
float a[max][max], sum=0; //inicijalizujemo sumu na 0
int n,i,j;

printf("Uneti ceo broj:\n");
	scanf("%d", &n);

if(n<=0 || n>max) { //provera da li je dimenzija u dozvoljenom opsegu
printf("Izlaz: ");	
printf("-1\n");
		return 1; // izlazak iz programa
}

printf("Uneti matricu:\n"); 
for(i=0; i<n; i++)
	for (j=0; j<n; j++)
		scanf("%f", &a[i][j]);

zbir(a, n, &sum); //poziv funkcije
printf("Izlaz: %f\n", sum);


return 0;
}
