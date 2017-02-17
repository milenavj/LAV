#include <stdio.h>
#define MAX_REC 21

int izbaci(char rec[], int k){	/*funkcija izbaci*/
	int i;
		
	if(k<0 || k >21)return 0;	/*provera k*/
	

	for(i=0;rec[i]!='\0';i++){		/*petlja koja prolazi kroz niz*/
		if(i>=k){

			rec[i]=rec[i+1];	/*dodeljuje sl clan niza*/
			}
		
	}
	if(i<=k) return 0;  			/*vraca netacno, ili tacno*/
	else return 1;
}


int main(){
	char rec[MAX_REC];	
	int poz, n;

	scanf("%d", &poz);	/*unosi se pozicija*/
	scanf("%s", rec);	/*unosi se rec*/

	
	if(izbaci(rec, poz)==1){	/*provera funkcije*/

	printf("%d\n", 1);		/*ispis rezultata*/
	printf("%s\n", rec);
	}
	else printf("%d\n", -1);

return 0;

}
