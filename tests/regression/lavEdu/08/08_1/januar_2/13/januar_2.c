#include <stdio.h>
#define size 21 // +1 za '/0'
#include <string.h> // za strlen

int strcspn1(char s[], char t[])
{
 int i,j;
 
 for (i=0; i<strlen(s); i++)
  for (j=0; j<strlen(t); j++)
   if(s[i]==t[j]) return i;  // poredim i-ti iz s sa svima iz t, pa i+1...
 return -1;
}

int main()
{
 char s[size], t[size];
 int a;
 
 printf("Unesite prvu nisku:\n");
 scanf("%s",&s);

 
 printf("Unesite drugu nisku:\n");
 scanf("%s",&t);
 
 a=strcspn1(s,t);
 
 printf("%d\n",a);

 return 0;
} 
