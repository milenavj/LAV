#include<stdio.h>
#define konstanta 100
float max_broj();
float aritmeticka_sredina();
float max_broj(float x[konstanta],int n)
{
int i;
float max;
max=x[0];
for(i=1;i<n;i++)
{
if(x[i]>max)
max=x[i];
}
return max;
}
float aritmeticka_sredina(float x[konstanta],int n)
{
int i;
float aritmeticka=0;
for(i=0;i<n;i++)
aritmeticka=aritmeticka+x[i];
aritmeticka=aritmeticka/n;
return aritmeticka;
}
int main()
{
int n,i;
float x[konstanta],max,sredina;
printf("Uneti dimenziju niza:");
scanf("%i",&n);
if(n>100||n<0)
{
printf("\nIzlaz:\n");
printf("-1\n");
}
else
{
printf("Elementi niza:");
for(i=0;i<n;i++)
scanf("%f",&x[i]);
max=max_broj(x,n);
printf("\nIzlaz:\n");
printf("\n%f",max);
sredina=aritmeticka_sredina(x,n);
printf("\n%f",sredina);
if(max>sredina*2)
printf("\n1\n");
else 
printf("\n0\n");
}
}




