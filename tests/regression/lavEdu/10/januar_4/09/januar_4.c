#include <stdio.h>

int main() {
	int n, i, b, j;
	scanf("%d", &n);
	if ((n%2)==0)
	printf("*\n");
	else {
	b = n;
	for (j=0; j<=n/2; j++){ 
		for (i=0; i<n; i++){
		if(i>=j && i<b)
		printf("x");
		else
		printf(".");
		}
	b--;
	printf("\n");
	}
	b--;
	for (j+=1; j<=n; j++){ 
		for (i=0; i<n; i++){
		if(i<j && i>=b)
		printf("x");
		else
		printf(".");
		}
	b--;
	printf("\n");
	} 
}

return 0;
}