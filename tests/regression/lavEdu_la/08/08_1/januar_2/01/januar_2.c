#include <stdio.h>
#include <string.h>
#define MAX 20

int strcspn1(char s[], char t[])
{ 
   int n,p;
   n = strlen(s);
   p = strlen(t);
   int i, j;
   for(i=0;i<n;i++)
      {
         for(j=0;j<p;j++)
            {if(s[i] == t[j])
                return i;}
 
}
    return -1;
}


int main()
{
 
 char a[MAX];
 char b[MAX];


 scanf("%s", a);
 scanf("%s", b); 
 printf("%d\n", strcspn1(a,b));

return 0;
} 



