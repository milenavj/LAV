#include<stdio.h>
#define max_size 21

int strcspn(char s[], char t[]){
	int i, j;
	
	for(i=0; s[i]; i++)
		for(j=0; t[j]; j++)
			if(s[i] == t[j])
				return i;

	return -1;
}


int main(){
	char s[max_size], t[max_size];
	scanf("%s", s);
	scanf("%s", t);

	printf("%d\n", strcspn(s, t));	
	return 0;
}

