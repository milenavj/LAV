#include <stdio.h>



int main()
{
float a11, a12, a21, a22, determinanta;

scanf("%f%f%f%f", &a11, &a12, &a21, &a22);
determinanta=(a11*a22)-(a12*a21);
printf("%f", determinanta);

return 0;
}






