#include <stdio.h>
#define DIMENZIJA 100

float max_broj(float a[], int n)
{
	int i;
	float max;
	max=a[0];
	for(i=1;i<n;i++)
		if(a[i]>max) max=a[i];
	return max;
}

float aritmeticka_sredina(float a[], int n)
{
	int i;
	float z=0,as;
	for(i=0;i<n;i++)
		z+=a[i];
	as=z/n;
	return as;
}

int main()
{
	int i,n;
	float a[DIMENZIJA],m,as;
	printf("Unesite dimenziju niza:\n");
	scanf("%d", &n);
	if (n>DIMENZIJA || n<0) 
		{printf("-1\n");
		return 0;}
	printf("Unesite niz:\n");
	for(i=0;i<n;i++)
		scanf("%f", &a[i]);
	m=max_broj(a,n);
	as=aritmeticka_sredina(a,n);
	printf("Maksimum datog niza je %f \n", m);
	printf("Aritmeticka sredina niza je %f \n", as);
	if (m>2*as) printf("1\n");
	else printf("0\n");
	return 0;
}
