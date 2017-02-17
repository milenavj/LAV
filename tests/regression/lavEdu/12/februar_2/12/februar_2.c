#include <stdio.h>

int main ()
{
  int a, b, c;
  
  printf ("Unesite 3 cela broja: ");
  scanf ("%d%d%d", &a, &b, &c);
  

  if (a>b && a>c)
    printf ("%d\n", a);
  else if (b>a && b>c)
    printf ("%d\n", b);
  else
    printf ("%\n", c);
  
  return 0;
}



