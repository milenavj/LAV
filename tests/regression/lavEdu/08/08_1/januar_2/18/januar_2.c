#include <stdio.h>

int strscpn(char s[], char t[]){
	int i,j;
	for(i=0;s[i]!='\0';i++){
		for(j=0;t[j]!='\0';j++){
			if(s[i]==t[j]) return i;
					}
	}
	return -1;
}

int main(){
	char prva[21], druga[21];
	int rez;
	

	scanf("%s", prva);
	scanf("%s", druga);
	
	rez=strscpn(prva,druga);

	if(rez>=0)printf("%d\n", rez);
	else printf("-1\n");
return 0;
} 


