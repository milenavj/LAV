#include <stdio.h>

int main(){
	int n,i,j,br=0, br2=0;

	scanf("%d", &n);
	if(n%2==0 || n<0){
	printf("*\n");
	return 0;
}

	for(i=0;i<n/2;i++){
		

		while(br<2*i/2){printf(".");
				br++;
				}
		
		while(br2<n-2*i){printf("x");
				br2++;
				}
		br2=0;
		br=0;
		while(br<2*i/2){printf(".");
				br++;
				}
		br=0;
		printf("\n");
		
}
	for(i=n/2;i>=0;i--){
		

		while(br<2*i/2){printf(".");
				br++;
				}
		
		while(br2<n-2*i){printf("x");
				br2++;
				}
		br2=0;
		br=0;
		while(br<2*i/2){printf(".");
				br++;
				}
		br=0;
		printf("\n");
		
}
		



}
