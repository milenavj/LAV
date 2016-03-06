#include<stdio.h>



int main(){
	unsigned n, i, j, korak=1;
	scanf("%u", &n);
	
	if(n%2 == 0){
		printf("*");
		return 0;
	}
	

	for(j=0; j<n; j+=korak){
		for(i=0; i<n; i++){
			if(i == n/2 && j == n/2){
				korak =-1;
				printf("x");
			}
			else if(i < j || i > n-1-j)
				printf(".");
			else
				printf("x");
		}
	printf("\n");
	}
	
	return 0;
}

