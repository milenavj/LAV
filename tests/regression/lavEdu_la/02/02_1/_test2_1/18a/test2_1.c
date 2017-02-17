#include<stdio.h>
#define MAX_DUZINA 100

float max_broj(float[], int);
float aritmeticka_sredina(float[], int);

main()
{
	float niz[MAX_DUZINA]; int n, i;
	printf("Uneti dimenziju niza: ");
	scanf("%d", &n);
	if(n<=0 || n>MAX_DUZINA)
	{
		printf("-1\n");
		return 0;
	}
	printf("Uneti niz: ");
	for(i=0; i<n; i++)
	{
		scanf("%f", &niz[i]);
	}
	printf("%f\n", max_broj(niz, n));
	printf("%f\n", aritmeticka_sredina(niz, n));
	if(max_broj(niz, n) > 2*aritmeticka_sredina(niz, n))
	{
		printf("1\n");
	}
	else
	{
		printf("0\n");
	}
    return 0;
} 

float max_broj(float niz[], int n)
{
	float max=niz[0]; int i;
	for(i=0; i<n; i++)
	{
		if(max<niz[i])
			max=niz[i];
	}
	return max;
}

float aritmeticka_sredina(float niz[], int n)
{
	float suma=0; int i;
	for(i=0; i<n; i++)
	{
		suma+=niz[i];
	}
	return (suma/n);
}