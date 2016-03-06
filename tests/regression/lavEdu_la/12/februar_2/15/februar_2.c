#include<stdio.h>
int main ()
{
  int a, b, c, max, max1;
  scanf("%d%d%d",&a, &b, &c);
  max=(a>b) ? a : b;
  max1=(c>max) ? c : max;
  printf("%d\n", max1);
  
  
  return 0;
}

