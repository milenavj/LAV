#include<stdio.h>
#define max_size 100
float max_broj(float a[], int n)
{
	int i;
	float max=a[0];
	for(i=0;i<n;i++)
		if(a[i]>max)
		max=a[i];
	return max;
}
float aritmeticka_sredina(float a[], int n)
{
	int i;
	float s=0;
	for(i=0;i<n;i++)
		s+=a[i];
	return s/n;
}
int main ()
{
	int i, n;
	float a[max_size];
	printf("Uneti dimenziju niza:\n");
	scanf("%d", &n);
	if(n<0 || n>max_size)
		{printf("-1\n");
		return 0;}
	printf("Uneti niz:\n");
	for(i=0;i<n;i++)
		scanf("%f", &a[i]);
	printf("\n%f\n%f\n", max_broj(a, n), aritmeticka_sredina(a, n));
	if(max_broj(a, n)>2*aritmeticka_sredina(a, n))
		printf("1\n");
	else
		printf("0\n");
return 0;
}



