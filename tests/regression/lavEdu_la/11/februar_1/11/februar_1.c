#include<stdio.h>
#define size 10


int main()

{

float a[size][size],b;
int i,j;

printf("Unesite brojeve\n");

for(i=0;i<2;i++)
  for(j=0;j<2;j++)
    scanf("%f",&a[i][j]);


b=a[0][0]*a[1][1]-a[0][1]*a[1][0];
  
printf("%.4f",b);
return 0;
}


