#include <stdio.h>

int main()
{
	int a;
	printf("Unesite pozitivan cetvorocifren broj: ");
	scanf("%i", &a);
	// provera da li je broj dobro unet
	if (a < 0) {
		printf("Rezultat je -1 \n");
		return 1;
	}
	if ( a < 1000 || a > 9999) {
		printf("Rezultat je -1 \n");
		return 1;
	}	
	// inicijalizujemo i redom rasporedjujem cifre broja a u x, y ,z, n
	int x,y,z,n;
	x = a / 1000;
	y = (a % 1000) / 100;
	z = ((a % 1000) % 100) / 10;
	n = ((a % 1000) % 100) % 10; 
	// proveravamo da li su brojevi rastuci
	if (x<y && y<z && z<n){
		printf("Rezultat je 1 \n");
		return 0;
	}
	// proveravamo da li su brojevi opadajuci
	if (x>y && y>z && z>n){
		printf("Rezultat je 2 \n");
		return 0;
	}
	//ako smo stigli do ovde broj je cetvorocifren ali neuredjen
	printf("Rezultat je 3 \n");	

	return 0;
} 
