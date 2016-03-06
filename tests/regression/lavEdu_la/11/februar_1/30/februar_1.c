#include<stdio.h>

int main(){
float a11, a12, a21, a22, r, m, n;
scanf("%f%f%f%f",&a11,&a12,&a21,&a22); /*ucitavamo brojeve*/
r=a11*a22;/*racunamo determinantu*/
m=a12*a21;
n=r-m; 
printf("%.4f\n", n); /*stampamo*/

} 
