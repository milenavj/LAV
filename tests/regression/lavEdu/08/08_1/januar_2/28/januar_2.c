#include<stdio.h>
#include<string.h>
int strcspn1(char s[],char t[])
  {
   int n,m,i,j,p=-1;
   n=strlen(s);
   m=strlen(t);                 
   for(i=0;i<n;i++)                        // Funkcija za proveru
   for(j=0;j<m;j++)
      {
       if(s[i]==t[j]) {
		       p=i;
		       return p;
		      }
      }
   return p;
  }
int main()
{
  char s[21],t[21];
  int p;
  printf("Unesite prvu nisku\n");
  scanf("%s",s);
  printf("Unesite drugu nisku\n");         //Unos niski
  scanf("%s",t);
  p=strcspn1(s,t);
  printf("\n%d",p);
  return 0;
}
