#include <stdio.h>
#include<stdio.h>

int main()
{
 int a, cj, cd, cs, ch, r;

 printf("Unesi cetvorocifren broj a : \n");
 scanf("%d", &a);

 cj=a%10;
 cd=(a%100)/10;
 cs=(a%1000)/100;
 ch=a/1000;

  if (a>=1000 && a<=9999)

   {

    r=(ch<cs && cs<cd && cd<cj) ? 1 : ((ch>cs && cs>cd && cd>cj) ? 2 : 3);

  
 
   }

  else if (a<1000 || a>9999)


    r=-1;

    printf("%d \n", r);


 return 0;


}



