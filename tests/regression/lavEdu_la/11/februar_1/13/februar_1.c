#include<stdio.h>

int main ()
{
float a11, a12, a21, a22, rez;
  
//printf("unesite a11:\n");	/*Pre svega unosimo sve brojeve koji se traze u zadatku*/
scanf("%f", &a11);

//printf("unesite a12:\n");
scanf("%f", &a12);

//printf("unesite a21:\n");
scanf("%f", &a21);

//printf("unesite a22:\n");
scanf("%f", &a22);

rez=a11*a22-a12*a21;

printf("%.4f\n", rez);		/*Pisemo %.4f, kako bi smo dobili ispisivanje samo 4 decimale*/

return 0;
}