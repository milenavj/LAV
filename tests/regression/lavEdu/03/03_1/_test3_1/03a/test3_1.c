#include<stdio.h>

#define MAX 21

void izmeni(char s[]){
	int i;
	for(i=0;s[i];i++) /* petlja koja prolazi kroz karaktere niske */
		if((i%2==0)&&(s[i]>='a')&&(s[i])<='z') s[i]=s[i]-'a'+'A'; /* proverava da li je karakter na */
			/*   parnom mestu i malo slovo i onda ga transformise u odgovarajuce veliko slovo*/
		else if((i%2==1)&&(s[i]>='A')&&(s[i]<='Z')) s[i]=s[i]-'A'+'a';
/*proverava da li je karakter na neparnom mestu i veliko slovo i onda ga transformise u odgovarajuce */
/* malo slovo*/
}

int main(){
	char s[MAX];
	printf("Unesi rec: ");
	scanf("%s",s);   /* ovo ucitava nisku    */
	izmeni(s);	/*  ovo pokrece funkciju izmeni   */
	printf("%s\n",s); /* ovo ispisuje nisku */
	return 0;
}


