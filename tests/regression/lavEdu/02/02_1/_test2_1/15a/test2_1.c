#include<stdio.h>
#define Max 100

float max_broj(int n,float a[])
{float max=a[0];
int i;

	
	for(i=0;i<n;i++)
	   if(a[i]>max)max=a[i];/*proveravanje da li je element veci od maximuma */
	

return max;
	
}

float aritmeticka_sredina(int n,float a[])
{float S=0;int i;

	
	for(i=0;i<n;i++)
	   S=S+(a[i]/n);/*arit. sredina je zbir elemenata podeljen sa njihovim brojem*/
	
	return S;
	
}



int main()
{int n,i;
float a[Max],maximum=0,A=0;/*deklaracija*/

	printf("Unesite dimenziju niza: ");
	scanf("%d",&n);/*unos broja elemenata*/
	
	if(n<0 || n>Max)/*ispitivanje da li je unos korektan*/
	{
		printf("-1\n");
		return 0;
	}
	
	printf("Uneti niz: ");/*unos elemenata*/
	for(i=0;i<n;i++)
	scanf("%f",&a[i]);

	maximum=max_broj(n,a);/*izracunanje maximuma*/
	A=aritmeticka_sredina(n,a);/*izracunavanje aritm.sredine*/
	
	printf("Izlaz:\n");/*stampanje izlaznih vrednosti*/
	printf("%f\n",maximum);
	printf("%f\n",A);	

	if(maximum > 2*A)/*isptivanje uslova*/
		printf("1\n");
	else
		printf("0\n");
}

