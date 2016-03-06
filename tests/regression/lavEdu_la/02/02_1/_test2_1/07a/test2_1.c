#include <stdio.h>
#define MAX_SIZE 100

float max_broj(float a[], int n)
{
	int i;
	float max;
	max = a[0]; /* maksimumu dodeljujemo vrednost prvog clana niza */
	for(i=1; i<n; i++)
		if(a[i]>max) /* ako je clan niza a[i] veci od trenutnog maksimuma, maksimum dobija vrednost a[i] */
			max = a[i];
	
	return max;/* funkcija vraca vrednost maksimuma */
}

float aritmeticka_sredina(float a[], int n)
{
	int i;
	float z=0, rez;
	for(i=0; i<n; i++) /* sabiraju se svi elementi niza*/
		z += a[i];
	
	rez = z/n; /*aritmeticka sredina se dobija deljenjem zbira svih clanova niza i broja elemenata niza*/
	
	return rez;
}

int main()
{
	int n, i;
	float a[100], max, as; /*max je vrednost maksimuma niza, a as vrednost aritmeticke sredine*/
	printf("Unesite ceo broj n:\n");
	scanf("%d", &n);
	if(n<=0 || n>MAX_SIZE) /*ako se unesu pogresne dimenzije niza, program stampa -1 */
		{
			printf("-1\n");
			return 0;
		}
	printf("Unesite n brojeva:\n");
	for(i=0; i<n; i++) /* petlja unosi elemente niza*/
		scanf("%f", &a[i]);
	max = max_broj(a, n); /*max dobija vrednost maksimuma niza*/
	as = aritmeticka_sredina(a, n); /*as dobija vrednost aritmeticke sredine niza*/
	printf("%f\n", max);
	printf("%f\n", as);
	
	if(max>2*as) /*uslov koji proverava da li je maksimum niza dvostruko strogo veci od aritmeticke sredine niza*/
		printf("1\n");
	else
		printf("0\n");

	return 0;
}



