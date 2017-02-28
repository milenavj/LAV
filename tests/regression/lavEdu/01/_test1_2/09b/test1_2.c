#include<stdio.h>
  int main () {
	int x,a,b,c,d,n;
	printf("Uneti pozitivan cetvorocifren broj:\n");
	scanf("%d",&x); //Unos podataka.
if (x>=1000 && x<=9999) //Ispituje da li je uneti broj cetvorocifren i pozitivan.
	{
  a=x%10; //Odvaja cifre.
  b=(x/10)%10;
  c=(x/100)%10;
  d=(x/1000)%10;

	if (a%2!=0) a=1; //Ispituje koje su parne, a koje nisu.
	if (b%2!=0) b=1;
	if (c%2!=0) c=1;
	if (d%2!=0) d=1;
		
	n=a*b*c*d;
		if (n!=1)
		printf("%d\n",n); //Ispisuje proizvod parnih cifara.
		else
		printf("1\n"); //Ukoliko nema parnih cifara, ispisuje 1 na std izlaz.
	}
else printf("-1\n"); //Ukoliko nije pozitivan cetvorocifren broj, ispisuje -1 na std izlaz.
return 0;
}