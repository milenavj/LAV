#include<stdio.h>

#define MAX 21

int strcspn(char s[],char t[]){
	int i,j;
	for (i=0;s[i];i++) // jedan po jedan karakter iz s pretrazuje
		for(j=0;t[j];j++) // jedan po jedan karakter iz t pretrazuje
			if (s[i]==t[j]) return i;  // vraca poziciju
	return  -1;   //ako ne nadje vraca -1
}

int main(){
	char s[MAX],t[MAX];
	scanf("%s",s);
	scanf("%s",t);
	printf("%d\n",strcspn(s,t));
	return 0;
}


