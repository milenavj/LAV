#include<stdio.h>
#include<string.h>
#include<stdlib.h>
int strcspn1(char s[], char t[])
{
  int i, j;
  for(i=0;i<strlen(s);i++)
  {
    for(j=0;j<strlen(t);j++)
    {
      if(s[i]==t[j])
	return i;
    }
  }
  return -1;
}

int main()
{
  char s[21];
  char t[21];
  scanf("%s",s);
  scanf("%s",t);
  printf("\n%d",strcspn1(s,t));
}

  