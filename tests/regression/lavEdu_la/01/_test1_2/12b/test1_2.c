#include<stdio.h>
  int main(){
	int a, a1, a2, a3, a4, pc=1;                      //deklaracija promenljivih
	printf("Unesite cetvorocifren pozitivan broj \n");
	scanf("%d", &a); //unos broja
	if(a>999 && a<10000){                            //provera ispravnosti unosa
		if ((a1=a%10) % 2 ==0) pc*=a1;           //odredjivanje pojedinacnih cifara broja i njihove parnosti
		if ((a2=(a/10)%10) % 2 ==0) pc*=a2;
		if ((a3=(a/100)%10) % 2 == 0)pc*=a3;
		if ((a4=a/1000) % 2 ==0) pc*=a4;;
	if(pc ==0) printf("Rezultat je: 0 \n");          // nula se racuna kao paran broj pa ukoliko je neka od cifara nula rezultat je sigurno 0
	else if(pc>1) printf("Rezultat je: %d ", pc);   //ispisivanje rezultata
	     else printf("Rezultat je 1 \n");
	}
  else printf("Rezultat je -1 \n");
  return 0;
}


