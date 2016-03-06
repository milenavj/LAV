#include <stdio.h>
#define MAX 100

float max_broj(float a[],int n)
{
	int i;
	float max;
	max=a[0];
		for (i=1;i<n;i++)
		max =(max>a[i]) ? max : a[i];

	return max;
}
	
float aritmeticka_sredina(float a[],int n)
	{
	int i;
	float s=0,as;
		for(i=0;i<n;i++)
		s=s+a[i];
	as=s/n;

	return as;
	}



int main()
{
	int n,i;
	float a[MAX],max,as;

	
printf("Uneti dimenziju niza: ");
scanf("%d", &n);
		if (n<1)
		printf("-1\n");
else

{
	for(i=0;i<n;i++)
	{
	printf("Unesite clan niza\n");
	scanf("%f",&a[i]);
	}

max=max_broj(a,n);
	printf("%f\n",max);
as=aritmeticka_sredina(a,n);
	printf("%f\n",as);

if(max>as*2)
printf("1\n");
	else
	printf("0\n");
}
return 0;


}
