#include<stdio.h>
#include<string.h>
#define MAXSIZE 20

int strcspn1(char s[], char t[])/*funkcija koja racuna poziciju*/
{
  int n = strlen(s);
  int m = strlen(t);
  
  int i, j;
  
  for(i=0; i<n; i++)
  {
    for(j=0; j<m; j++)
    {
      if(s[i] == t[j]) return i;
      
    }
    
  }
return -1;
}

int main()
{
  char s[MAXSIZE], t[MAXSIZE]; /*definisu se niske*/
  
  printf("Unesi prvu rec\n");/*unose se reci*/
  scanf("%s", s);
  
  printf("Unesi drugu rec\n");
  scanf("%s", t);
  
  printf("%d\n", strcspn1(s, t));/*ujedno se poziva funkcija i stampa se izlaz*/
  
return 0;  
}
