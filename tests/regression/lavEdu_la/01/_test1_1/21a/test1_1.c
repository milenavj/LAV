#include <stdio.h>

int main() {
	int broj, cif1, cif10, cif100, cif1000;
	printf("Unesi jedan pozitivan cetvorocifreni broj:\n");
	scanf("%d", &broj);
	cif1 = broj % 10;
	cif10 = (broj % 100) / 10;
	cif100 = (broj / 100) % 10;
	cif1000 = broj / 1000;
	if ((broj < 1000) || (broj >= 10000)) 
	printf("-1\n");
	else {
	   if ((cif1000 < cif100) && (cif100 < cif10) && (cif10 < cif1))
	   printf("1\n");
	   else {
	      if ((cif1000 > cif100) && (cif100 > cif10) && (cif10 > cif1))
	      printf("2\n");
	      else 
	      printf("3\n");
	   }
	}
	return 0;
} 
