#include <stdio.h>
#include <string.h>

int strscpn1(char s[], char t[]){
int i,j;
for(i=0;s[i];i++){
    for(j=0;t[j];j++)
if (s[i]==t[j])

  return i;
}  
return -1;

}

int main(){

char s[20],t[20];

printf("Unesi prvu nisku:");
scanf("%s", s);

printf("Unesi drugu nisku:");
scanf("%s", t);


printf("%d\n",strscpn1(s,t));


return 0;

}