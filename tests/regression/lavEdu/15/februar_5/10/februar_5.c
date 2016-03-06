# include <stdio.h>

int main ()
{
  int n, i, x;
  
  scanf ("%d", &n);
  
  if (n<=0)
  {
    printf ("-1 \n");
    return 0;
  } 

  x=1;
  while (n!=0)
  {
    for ( i=x; i<=n; i++)
      printf ("%d ", i);
    n--;
    x++;
  }

 printf ("\n");
 
 return 0;
}  



  
