#include<stdio.h>
#define MAX 10

void maksimumi(int matrica[][MAX],int niz[],int n,int m)
{
int i,j;
for(i=0;i<n;i++)
{
	niz[i]=matrica[i][0];/*Postavljanje prve vrednosti na maksimum*/
	for(j=0;j<m;j++)
		if(matrica[i][j]>niz[i])/*Provera maksimuma*/
			niz[i]=matrica[i][j];
}
}
int main()
{
int n,m,matrica[MAX][MAX],niz[MAX],i,j;
printf("Unesite broj vrsta: \n");
scanf("%d",&n);/*Unos vrsta matrice*/
printf("Unesite broj kolona: \n");
scanf("%d",&m);/*Unos kolona matrice*/
if(n<=0||n>=MAX||m>=MAX||m<=0)/*Provera tacnosti unosa*/
	{
	printf("-1");
	return -1;
	}
printf("Unesite elemente matrice: \n");
for(i=0;i<n;i++)
	for(j=0;j<m;j++)
		scanf("%d",&matrica[i][j]);/*Unos elemenata matrice*/
maksimumi(matrica,niz,n,m);/*Poziv funkcije*/
for(i=0;i<n;i++)
	printf("%d ",niz[i]);/*Izlaz*/

return 0;
}



