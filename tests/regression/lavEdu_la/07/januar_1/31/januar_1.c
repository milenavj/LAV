#include <stdio.h>
int veci(float a, float b){

	if(a>b) return 1;
	else return 2;
}

int main(){
float a, b;
int r;
printf("Ucitaj dva broja: ");
scanf("%f%f", &a, &b);
 
r=veci(a,b);
printf("%d\n", r);
}
