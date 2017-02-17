#include<stdio.h>

int main()
{
	int broj,cifj,cifd,cifs,cifh;
	printf("Unesite pozitivan cetvorocifren broj: \n");
	scanf("%d",&broj);//unos broja
	if(broj<=9999&&broj>=1000)//Piranje da li je broj cetvorocifren
	{
		cifh=broj/1000;//cifra hiljada
		cifs=(broj%1000)/100;//cifra stotina
		cifd=(broj%100)/10;//cifra desetica
		cifj=(broj%100)%10;//cifra jedinica
		if(cifh<cifs&&cifs<cifd&&cifd<cifj)
			printf("Rezultat je: 1 \n");//broj je rastuci
		else if(cifh>cifs&&cifs>cifd&&cifd>cifj)
			printf("Rezultat je: 2 \n");//broj je opadajuci
		else if(cifh==cifs&&cifs==cifd&&cifd==cifj)
			printf("Rezultat je: 3 \n");//broj ima iste cifre
		else if((cifh<cifs&&cifs<cifd&&cifd<cifj)==0)
			printf("Rezultat je: 3 \n");//ne odredjeni poredak
		
	}
	else if(broj<0)//pitanje da li je broj negativan
		printf("Rezultat je: -1 \n");//broj je negativan
	else if(broj<1000||broj>9999)//pitanje da li broj nije cetvorocifren
		printf("Rezultat je: -1 \n");//broj nije cetvorocifren
}

