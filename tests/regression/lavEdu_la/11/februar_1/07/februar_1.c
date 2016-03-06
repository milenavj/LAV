#include <stdio.h>

	int main()
{

	float a11, a12, a21, a22,a;

	printf("Unesite vrednosti\n");
	scanf("%f%f%f%f",&a11, &a12, &a21, &a22);

	a= a11*a22 - a21*a12;

	printf("Rezultat je  %.4f\n",a);








return 0;
}
 
