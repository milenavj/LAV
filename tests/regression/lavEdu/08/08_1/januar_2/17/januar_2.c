#include<stdio.h>
#include<string.h>
#define SIZE 21

int strcspn1(char s[], char t[])
{
 int i,j;
  for(i=0; s[i]; i++) 
      for(j=0; t[j]; j++)
	 if( s[i]==t[j])  //prolazimo kroz obe niske i uporedjujemo clanove
	  return i;	//	
 return -1; 
}

int main()
{
  char t[SIZE],s[SIZE]; //deklaracija niski
  
  scanf("%s",s); //unos prve niske
  scanf("%s",t); //unos druge niske
  
  printf("%d\n",strcspn1(s,t)); //stampanje vrednosti koje vraca funkcija
  
  
  
  
  return 0;
}



