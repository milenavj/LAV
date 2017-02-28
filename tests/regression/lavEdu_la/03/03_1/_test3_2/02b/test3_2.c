#include <stdio.h>
#define DIMENZIJA 21
// simbolicka konstanta

void izmeni (char niska[]); //deklaracije funkcija
void unos (char niska[]);
void stampa (char niska[]);

int main ()
{
	char niska[DIMENZIJA];
	
	unos (niska);
	izmeni (niska);
	stampa (niska);


	return 0;
}


void izmeni (char niska[]) //funkcija za izmenu niske, jedini argument funkcije je niska koja se zavrsava terminirajucom 			   //nulom
{
	int i;
	
	for (i  = 0; niska[i]; i++)
		if (i % 3 == 0 && niska[i] >= 'a' && niska[i] <= 'z') // ako je indeks deljiv sa 3 proverava da li je
			niska[i] -= ('a' - 'A');		      // karakter malo slovo i ako jeste pretvara ga u veliko
		else 
			if (i % 3 == 1 && niska[i] >= 'A' && niska[i] <= 'Z')//ako je ostatak pri deljenju indeksa 1
				niska[i] += ('a' - 'A');		     //onda proverava da li karakter veliko slovo
									     // i ako jeste pretvara ga u malo slovo
}

void unos (char niska[])//funkcija za unos niske
{
	int i;
	
	printf("Unesi rec: ");

	for (i = 0; i < DIMENZIJA && niska[i - 1] != '\n'; i++)
		scanf("%c", &niska[i]);

	niska[i - 1] = '\0';
}

void stampa (char niska[])//funkcija za ispisivanje niske
{
	int i;
		

	for (i = 0; niska[i]; i++)
		printf("%c", niska[i]);

	printf("\n");
}

