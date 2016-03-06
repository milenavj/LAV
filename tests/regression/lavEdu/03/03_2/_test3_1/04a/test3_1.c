#include<stdio.h>
#define SIZE 21 

void izmeni(char rec[])/*funkcija koja menja elemente niske koji su slova */
{
int i;
	for(i=0;rec[i];i++)
		if(i%2==0 && rec[i]>='a' && rec[i]<='z')rec[i]=rec[i]-'a'+'A';/*ovde odredjujemo parne elemente niske koji su mala slova i menjamo ih u velika*/
		else if(i%2!=0 && rec[i]>='A' && rec[i]<='Z' )rec[i]=rec[i]-'A'+'a';/*ovde odredjujemo neparne elemente niske koji su velika slova i menjamo ih u mala*/
		else rec[i]=rec[i];/*ovde su  elementi koji ne odg gornjim uslovima i njih ne menjamo*/
}

int main()
{char rec[SIZE];/*deklaracija promenljivih u glavnom programu*/
	printf("Unesi rec:");
	scanf("%s",rec);/*unos reci*/
	izmeni(rec);/*koristimo funkciju izmeni u glavnom programu*/
	printf("%s\n",rec);/*ispis reci*/
	

}


 
