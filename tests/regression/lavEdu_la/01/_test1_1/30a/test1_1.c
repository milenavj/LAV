#include <stdio.h>
int main ()
{
	int x, j, d, s, h, rezultat;
	printf("Unesite pozitivan cetvorocifren broj:\n");
	scanf("%d", &x);/*Unosi se broj.*/
	j = x%10;/*Racuna cifru jedinica.*/
	d = x%100/10;/*Racuna cifru desetica.*/
	s = x%1000/100;/*Racuna cifru stotina.*/
	h = x/1000;/*Racuna cifru hiljada.*/
	
	if(x<1000 || x>9999) printf("Rezultat je: -1.\n");/*Proverava da li je broj cetvorocifren.*/
	else
	{
		rezultat = (h<s && s<d && d<j) ?  1 : (h>s && s>d && d>j) ? 2 : 3;/*Proverava da li su cifre u rastucem, opadajucem redosledu ili nisu uredjene.*/
		printf("Rezultat je: %d\n", rezultat);/*Stampa rezultat.*/
	}
	
	return 0;
}




