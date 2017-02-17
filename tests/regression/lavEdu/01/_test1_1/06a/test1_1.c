#include <stdio.h>

int main ()
{
/*deklaracije*/
int x, rez, c1, c2, c3, c4; 

/*Unos broja*/
printf("Unesi jedan pozitivan cetvorocifren broj: ");
scanf("%d", &x);

if (x<1000 || x>9999)
	rez=-1;
	else
		{
		/*Rastavljanje broja na cifre*/
		c1=x/1000;
		c2=x%1000/100;
		c3=x%100/10;
		c4=x%10;
		/*ispitivanje uredjenosti koriscenjem pravila tranzitivnosti x<y && y<z => x<z*/
		rez = (c1<c2 && c2<c3 && c3<c4) ? 1 : (c1>c2 && c2>c3 && c3>c4) ? 2 : 3;
		}
printf("Rezultat je: %d\n", rez);

return 0;
}
