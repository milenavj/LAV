#include<stdio.h>

#define MAX 21


int main(){
	int n,i,j,k=0,l=1;
	scanf("%d",&n);
	if(n%2==1)
	{
	for(i=0;i<n;i++){
	 for(j=0;j<n;j++){
			if ((j<k)||(j>=n-k)) printf(".");
			else printf("x");
			 }
		if ((k!=0)&&(n/(k+1)==1)) l=-1;
		k=k+l;
		printf("\n");
			}
	}
	else printf("*\n");
	return 0;
}


