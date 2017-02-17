#include <stdio.h>


int strcspn(char s[], char t[]){

	int i;
	int j;
	
	for(j = 0; s[j]; j++){

		for(i = 0; t[i]; i++){
			if(s[j] == t[i])
				return j;
		}
	}

	return -1;


}

int main(){
	char s1[20];
	char s2[20];

	
	scanf("%s", s1);

	scanf("%s", s2);

	printf("%d\n", strcspn(s1, s2));	



	return 0;
} 
