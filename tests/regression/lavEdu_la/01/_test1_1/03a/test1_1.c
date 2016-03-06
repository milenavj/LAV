#include <stdio.h>

int main(){

	int broj;  //broj koji se unosi

	int c1,c2, c3, c4;  //cifre cetvorocifrenog broja, s leva na desno

	printf("Unesite pozitivan cetvorocifren broj: \n");
	
	scanf("%d", &broj);

	printf("\n");

	if((broj > 9999) || (broj < 1000))		// ispitijemo da li smo uneli cetvorocifren pozitivan broj
		printf("Rezultat je: -1\n");
	else
		{
			c1 = broj/1000;			//racunanje cifara
			c2 = (broj/100) % 10;
			c3 = ((broj/10) % 100) % 10;
			c4 = broj % 10;

			if((c1 < c2) && (c2 < c3) && (c3 < c4))			//ispitujemo da li su cifre uredjene
				printf("Rezultat je: 1\n");
			else if((c1 > c2) && (c2 > c3) && (c3 > c4))
				printf("Rezultat je 2\n");
			else
				printf("Rezultat je: 3\n");

		}

	return 0;

}

	