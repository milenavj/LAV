#include <stdio.h>

#define MAX_NIZ 100


float max_broj(float a[], int n){

	int i;		/* brojac */
	float max;	/* maksimum niza */
	
	
	
	max = a[0];	/* pretpostavljamo da je prvi element niza maksimum */

	for(i = 1; i < n; i++){

		if(max < a[i])		
			max = a[i];
	}

	return max;

}

float aritmeticka_sredina(float b[], int m){

		float suma, as;		/* suma elemenata niza i aritmeticka sredina, respektivno */
		int j;			/* brojac */

		suma = 0;

		for(j = 0; j < m; j++)
			suma += b[j];

		as = suma/m;

		return as;
}






int main(){
	
	float g[MAX_NIZ];	/* nas niz */
	int n;			/* dimenzija niza */
	int i;			/* brojac */
	float max, arit;	/* promenljive koje sadrze maksimum niza i aritmeticku sredinu niza */

	printf("Unesite dimenziju niza: \n");
	scanf("%d", &n);

	printf("\n");

	if(n <= 0)
		printf("%d\n", -1);
	else{

		printf("Unesite realne brojeve niza: \n");

		for(i = 0; i < n; i++)
			scanf("%f", &g[i]);

		max = max_broj(g, n);
		arit = aritmeticka_sredina(g, n);

		printf("Maksimum unetog realnog niza iznosi: %f\n", max);
		printf("Aritmeticka sredina unetog realnog niza iznosi: %f\n", arit);

		if(max > 2 * arit)
			printf("%d\n", 1);
		else
			printf("%d\n", 0);
		}

	
	return 0;
} 
