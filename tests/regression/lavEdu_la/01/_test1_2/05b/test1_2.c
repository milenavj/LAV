#include<stdio.h>

int main()
{
	int broj, jedinice, desetice, stotine, hiljade, parna1, parna2, parna3, parna4, proizvod;

		/*trazimo od korisnika da unese broj*/
	printf("Unesite pozitivan cetvorocifreni broj: \n");
	scanf("%i", &broj);
		/*ispitujemo da li je korisnik uneo cetvorocifreni broj*/
		/*broj < 1000 takodje nam osigurava da korisnik ne unese negativan broj*/
	if ((broj < 1000) || (broj > 9999))
		printf("Rezultat je: -1 \n");
	else
		{
		/*prvo odvajamo cifre*/
		jedinice = broj % 10;
		desetice = (broj % 100) / 10;
		stotine = (broj % 1000) / 100;
		hiljade = broj / 1000;

		/*potom ispitujemo da li je neka od ovih cifara parna*/
		/*koje nisu parne, njima dodeljujemo vrednost 1, jer se svaki broj pomnozen sa 1 ne menja*/
		if (jedinice % 2 == 0)
			parna1 = jedinice;
		else parna1 = 1;
		if (desetice % 2 == 0)
			parna2 = desetice;
		else parna2 = 1;
		if (stotine % 2 == 0)
			parna3 = stotine;
		else parna3 = 1;
		if (hiljade % 2 == 0)
			parna4 = hiljade;
		else parna4 = 1;
		
		/*posto smo izdvojili parne, racunamo njihov proizvod*/
		proizvod = parna1 * parna2 * parna3 * parna4;
		
		/*i stampamo rezultat*/
		printf("Rezultat je: %i \n", proizvod);
		}

return 0;
}
