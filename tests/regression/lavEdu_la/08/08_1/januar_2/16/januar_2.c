#include<stdio.h>
#include<string.h>

int strcspn1(char s[], char t[])
{
  int i, j, n, m;
  n = strlen(s);
  m = strlen(t);
  
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
  char s[20], t[20];
  int a;
  
  scanf("%s", s);
  
  scanf("%s", t);
  
  a = strcspn1(s, t);
  
  printf("%d\n", a);
  
  return 0;
}
  
  
  
  
  
  
  