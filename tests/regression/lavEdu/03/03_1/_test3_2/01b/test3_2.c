
#include<stdio.h>



void izmeni(char s[]){


	int i=0;
	while(s[i] )

{	if((i%3==0) && (s[i]>='a' && s[i]<='z') )
	 s[i]=s[i] - ('a' - 'A');
         if((i%3==1) && (s[i]>='A' && s[i]<='Z'))
	  s[i]=s[i] + ('a' - 'A');
	
	
	i++;
}

}

void ispisi(char s[]){

	int i;
	for(i=0; s[i]; i++)
	 printf("%c", s[i]);
	 printf("\n");

}

int main(){



	char s[20];
	printf("Uneti string: \n");
	scanf("%s", s);
	izmeni(s);
	ispisi(s);




	return 0;

}
