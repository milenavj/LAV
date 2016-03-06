#include <stdio.h>
int main () {
	
	int broj,prva,druga,treca,cetvrta;
	
	printf("Unesi jedan pozitivan cetvorocifren broj: ");
	scanf("%d", &broj);

	

	if (broj<1000 || broj>9999) {
		printf("Rezultat je: -1\n");
	}
	else {
		
		prva=broj/1000;
		druga=broj/100%10;
		treca=broj/10%10;
		cetvrta=broj%10;

		if (prva%2 != 0)
			prva=1;
		if (druga%2 != 0)
			druga=1;
		if (treca%2 != 0)
			treca=1;
		if (cetvrta%2 != 0)
			cetvrta=1;

		printf("Rezultat je : %d\n", prva*druga*treca*cetvrta);
	
	}

	return 0;

}
