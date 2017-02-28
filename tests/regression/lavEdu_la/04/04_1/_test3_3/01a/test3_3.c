#include <stdio.h>
#define SIZE1 10
#define SIZE2 10


void maksimumi( int matrica[][SIZE2], int n, int m, int niz_maksimuma[] )
{
	int i,j;
		for(i=0; i<n; i++)
			{
			niz_maksimuma[i] = matrica[i][0];
			for(j=1; j<m; j++)
				if( matrica[i][j] > niz_maksimuma[i] )
					niz_maksimuma[i] = matrica[i][j];
					
			}
			


}



int main()
{
	int matrica[SIZE1][SIZE2],n,m,i,j,niz_maksimuma[SIZE1]; // n,m - promenljive za unos dimenzija   i,j - brojacke promenljive

		printf("Unesi broj vrsta: ");
		scanf("%d", &n);

		printf("Unesi broj kolona: ");
		scanf("%d", &m);

		if( n<=0 || n>SIZE1 || m<=0 || m>SIZE2 ) //Provera ispravnosti dimenzija matrice
			{
			printf("Izlaz:\n-1\n");
			return 0;
			}
		else
			{

			printf("Unesi elemente matrice:\n"); 
		
			for(i=0; i<n; i++) //Unos matrice
				for(j=0; j<m; j++)
					scanf("%d", &matrica[i][j] );
		
				
	
			maksimumi(matrica,n,m,niz_maksimuma); //Poziv funkcije

			printf("Izlaz:\n");		
			for(i=0; i<n; i++)
				printf("%d ", niz_maksimuma[i]);
			printf("\n");

			}		


return 0;
}
