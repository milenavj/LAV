#include<stdio.h>
#include<string.h>
#define MAX 21
/*Ovde se pishe funkcija za izbacivanje,poziva se rech i pozicija za izbacivanje*/
/*promenljiva rec je uneta rech, i je lokalna ,koristi se u for-u, k je pozicija izbacivanja*/
int izbaci(char rec[], int k)
{
	int i;
	if(k+1>strlen(rec))
		return -1;
	for(i=0;i<strlen(rec);i++)
	{
		if(i>=k)
		{
			rec[i]=rec[i+1];
		}
	}
	return 1;
}
int main()
{
char rec[MAX];
int k;
printf("\n Unesi poziciju:");
scanf("%d",&k);
printf("\n Unesi rec:");
scanf("%s",&rec);
/*Ovde se pita da li je izlaz 1 ili -1 i na osnovu toga se pishe reshenje i nova rech ako je izlaz 1*/
if(izbaci(rec,k)==-1)
	printf("\n-1\n");
else
	printf("1\n %s\n",rec);
}
