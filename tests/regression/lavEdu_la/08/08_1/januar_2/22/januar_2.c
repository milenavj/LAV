#include<stdio.h>
#define MAX 20
strcspn1(char s[],char t[]){
int i,j;
 for(i=0;s[i];i++)
  for(j=0;t[j];j++){
    if(s[i]==t[j]){
	return i;
	  exit(1);
}
}
return -1;
}
int main(){char a[MAX],b[MAX];
printf("unesi prvi string:");
  scanf("%s",&a);
printf("unesi drugi string");
  scanf("%s",&b);
printf("%d\n",strcspn1(a,b));
return 0;
}