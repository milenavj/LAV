#include<stdio.h>
#define MAX_DIMENZIJA 100    
/*Funkvija za unos tipa void jer ne vraca vrednost*/
void unos(float a[],int n)
{
	int i;
	for(i=0;i<n;i++)
	{
//		printf("a[%d]= ",i);
		scanf("%f",&a[i]);
	}
}
/*f-ja za maximum tipa float jer vraca maksimalni element niza ciji su elementi tipa float*/
float maximum(float a[],int n)
{
float max=a[0];

  int i;
  for(i=1;i<n;i++)
    {
      if(a[i]>max)
       max=a[i];
    }
return max;
}
/*f-ja za aritmeticku sredinu tipa float*/
float aritmeticka(float a[],int n)
{
int i;
float s=0;
for(i=0;i<n;i++)
{
 s+=a[i];
}
return s/n;
}
int main()
{
float a[MAX_DIMENZIJA];  /*Dimenzija niza preko simbolicke konstante*/
int n;
printf("Unesite broj elemenata niza:");
scanf("%d",&n);
if(n<0 || n>MAX_DIMENZIJA)  /*ako je dimenzija negativna ili veca od maksimalne dimenzije vraca se -1*/
  printf("-1\n");
else{
  unos(a,n);         /*poziv f-je za unos niza*/
  printf("\n%f", maximum(a,n));        /*poziv f-ja za maksimum i aritmeticku sredinu.*/
  printf("\n%f \n", aritmeticka(a,n));
  if(maximum(a,n)>2*aritmeticka(a,n))  /*Ispituje se da li je maksimum dvostruko strogo veci od aritmeticke sredine i ispisuje se odgovarajuci rezultat*/
     printf("1\n");             /*ako je gornji uslov ispunjen ispisuje se 1*/
  else printf("0\n");           /*ako gornji uslov nije ispunjen ispisuje se 0*/
}
return 0;      /*f-ja main vraca vrednost integer*/
}
