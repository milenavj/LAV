#include <stdio.h>
int main()
{
	int Cj,Cd,Cs,Ch,n;
	printf("Unesi cetvorocifren broj:\n");
	scanf("%d",&n);

	if(n>999 && n<10000) {
	Cj=n%10;
	Cd=(n%100)/10;
	Cs=(n/100)%10;
	Ch=(n/1000);
	
	

	if(Ch>Cs && Cs>Cd && Cd>Cj){printf("2\n");}
	else if(Cj>Cd && Cd>Cs && Cs>Ch){printf("1\n");}
	else { printf("3\n");}
	}
	else {printf("-1\n");}	


	return 0;
}
