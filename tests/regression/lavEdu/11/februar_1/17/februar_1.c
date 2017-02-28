#include<stdio.h>
main()
{
float a11, a12, a21, a22;
printf("Unesite cetiri broja za determinantu:\n");
scanf("%f%f%f%f", &a11, &a12, &a21,&a22);
/*Obrazac za izracunavanje determinante glasi ad-bc, dakle :*/
printf("Determinanta iznosi: %f\n", a11*a22-a12*a21);

return 0;
}
