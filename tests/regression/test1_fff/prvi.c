#include<stdlib.h>

struct tacka
{
int x;
int y;
};

main()
{
int a[10];
int k = 5;
int b[30];
float c[7];
struct tacka t[20];

k = 15;
t[k].y = 2; /*SAFE*/
k = 0;
t[k].y = 2; /*SAFE*/
t[k].x = 5; /*SAFE*/
k = 20;
t[k].y = 2; /*FLAWED*/


a[2] = 5; /*SAFE*/
a[3] = a[2]; /*SAFE*/ /*SAFE*/

k = 3;
a[k] = 3; /*SAFE*/
k = 15;
a[k] = 3; /*FLAWED*/

if(k>0)
	k = -2;
a[k] = 3; /*FLAWED*/

if(k>0)
	k = 2;
a[k] = 3; /*FLAWED*/

a[12] = 3;/*FLAWED*/
b[4] = 0; /*SAFE*/
a[3] = 5; /*SAFE*/
if(k>0)
	c[4] = 8; /*UNREACHABLE - SAFE*/
c[10] = 5; /*FLAWED*/
t[4].x = 2; /*SAFE*/
t[4].y = 3; /*SAFE*/
t[30].x = 2; /*FLAWED*/

}

