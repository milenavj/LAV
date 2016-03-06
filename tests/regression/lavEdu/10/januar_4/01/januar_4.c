#include <stdio.h>


int main(){
	int n;
	int i, i2;
	int j = 0;
	int k;

	scanf("%d", &n);

	

	if(n % 2 == 0)
		printf("*\n");
	else{
		for(i2 = 0; i2 < (n/2) + 1; i2++){
			
				for(i = 0; i < i2; i++)
					printf(".");
	
				for(i = 0; i < n - 2 * i2; i++)
					printf("x");

				for(i = 0; i < j; i++)
					printf(".");
	
				printf("\n");
	
				j++;

				

		}

		for(i2 = (n/2) - 1; i2 >= 0; i2--){

				for(i = 0; i < i2; i++)
					printf(".");

				for(i = 0; i < n - 2 * i2; i++)
					printf("x");

				for(i = 0; i < i2; i++)
					printf(".");

				printf("\n");
		}
	}		
			

	return 0;
} 
