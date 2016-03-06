 
#include<stdio.h>
#define MAX_SIZE 21
int izbaci(char rec[], int pozicija)
{	  // funkcija za proveru duzine reci 
	int i=0,j=0;
	while(rec[i])
	{
		j++;
		i++;
	}
	i=0;
	if(j>pozicija)
	{
		return 1;
		
	}
	else return 0;
	
}
int main()
{
	char rec[MAX_SIZE];
	int i=0,pozicija;
	printf("Unesi poziciju: ");
	scanf("%d",&pozicija);
	printf("Unesi rec: ");
	scanf("%s",&rec);
	printf("Izlaz: \n");
	if(pozicija>=0){ 		 //provera korektnosti podataka
	if(izbaci(rec,pozicija)){	//provera da li moze da se izbaci slovo iz reci
	printf("%d \n",izbaci(rec,pozicija));
	while(rec[i])
		{
			if(i>=pozicija&&rec[i]!='\0') rec[i]=rec[i+1];
			
			i++;
		}
	printf("%s \n",rec);   //stampa
	}
	else printf("-1 \n");
	}
	else printf("-1 \n");
	return 0;
	
}



