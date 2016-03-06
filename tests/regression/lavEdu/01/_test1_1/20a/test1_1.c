#include <stdio.h>
int main ()
{
int x, y, a, b, c, d;
	printf("Unesite jedan pozitivan cetvorocifren broj: \n");
	scanf("%d", &x);
	if (x>9999 || x<1000)
	{
		y=-1;
		printf("Rezultat je: %d\n", y);
	}
	else
	{
		a=x/1000; //a, b, c, d su mi cifre u cetvrocifrenom broju: 1234=abcd//
		b=(x/100)%10;
		c=(x/10)%10;
		d=x%10;
		if (a>b && c>d && b>c)
		{
			y=2;
			printf("Rezultat je: %d\n", y);
		}
		else if (d>c && b>a && c>b)
		{
			y=1;
			printf("Rezultat je: %d\n", y);
		}
		else
		{
			y=3;
			printf("Rezultat je: %d\n", y);
		}
	}
return 0;
}
