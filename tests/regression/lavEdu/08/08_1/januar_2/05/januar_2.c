#include<stdio.h>

int strcspn(char s[], char t[]) {
	int i, j;
	for(i=0;s[i];i++)
		for(j=0;t[j];j++)
			if(s[i]==t[j]) return i;
return -1;
}

int main() {
char s[21], t[21];
scanf("%s", &s);
scanf("%s", &t);
printf("%d\n", strcspn(s, t));	

return 0;
}
