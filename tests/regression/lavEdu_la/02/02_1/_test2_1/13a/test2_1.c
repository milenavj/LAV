#include<stdio.h>
#define MAX 10

float max_broj(float a[],int n)
{
int i;
float max=a[0];
for(i=1;i<n;i++)
	if(max<a[i])
		max=a[i];
return max;
}

float aritmeticka_sredina(float a[],int n)
{
int i;
float s=0;
for(i=0;i<n;i++)
	s+=a[i];
return s/n;
}

int main()
{

int n,i;
float a[MAX];
printf("Unesite n\n");
scanf("%d",&n);
if(n>0)
{
printf("Unesite brojeve\n");
for(i=0;i<n;i++)
	scanf("%f",&a[i]);
printf("Maksimum je %f\n",max_broj(a,n));
printf("Aritmeticka sredina je %f\n",aritmeticka_sredina(a,n));
if(max_broj(a,n)>(2*aritmeticka_sredina(a,n)))
printf("1\n");
else printf("0\n");
}
else printf("-1\n");

}

