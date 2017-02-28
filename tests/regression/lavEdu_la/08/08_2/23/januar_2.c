#include<stdio.h>
#define MAX 20
int strcspn(char s[],char t[]){
	int i,j,p1,p2=MAX;
	for(i=0;t[i];i++){
		for(j=0;s[j];j++){
			if(s[j]==t[i]){
					p1=j;
					if(p1<p2)p2=p1;
					}
				}
		}
	if(p2==MAX)return -1;
	else return p2;
}
main(){
	char s[MAX],t[MAX];
	int f;
	printf("Unesite prvu nisku: ");
	scanf("%s",s);
	printf("Unesite drugu nisku: ");
	scanf("%s",t);
	f=strcspn(s,t);
	printf("Izlaz: %d\n",f);
	}


