#include <stdio.h>
#define SIZE 21

// Funkcija izbaci koristi petlju u kojoj pocinje da pregleda nisku 
// od pozicije ispred zadatog k i stavlja ga na prethodno mesto
void izbaci(char *s,int k){
int i;
for(i=k+1;s[i];i++)s[i-1]=s[i];

}


int main(){
 char s[SIZE];
 int k,i,j;
 scanf("%d",&k);
 scanf("%s",s);
  for(i=0;s[i];i++);

// uslov gde se prekida program u slucaju negativnog broja, prekoracenja ,ili pozicije van niske
 if(k<0||k>=SIZE||k>=i){
   printf("-1\n");
   return 0;
 }
izbaci(s,k);

 printf("1\n");

 for(j=0;j<i-1;j++)printf("%c",s[j]);

printf("\n");



return 0;
} 
