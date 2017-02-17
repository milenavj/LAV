#include <stdio.h>

int strcspn1(char s[],char t[]){
	int i,j,pom;
	for(i=0;s[i];i++){
		for(j=0;t[j];j++)
			if(s[i]==t[j])  return i;   //(pom=i; break;)
	}
			return -1;



}

int main() {
char s[20];
char t[20];
int n;
scanf("%s",s);
scanf("%s",t);

printf("%d\n",strcspn1(s,t));


}
