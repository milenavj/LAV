int  main()
{


unsigned  int a[10];
unsigned  int *p;
unsigned  int *q;
unsigned  i = 1;
unsigned  j = 4;
unsigned  k  = 3;

p = a+2; /*SAFE*/

p[10] = 5; /*a[12] => FLAWED*/

p[13] = a[5]; /*a[15] => FLAWED, a[5] => SAFE*/


q = a + 5; 
q[j] = 2; 	/*a[9] => ako je q short safe else flawed*/


p[1] = 3; /*a[3] => SAFE*/
p[5] = 3; /*a[7] => SAFE*/


if(i>j) p++; /*UNREACHABLE*/
else q++;    /* SAFE */


q[j] = 12;  /*a[10] => FLAWED*/
q[i-10] = 19; /*a[-3] => FLAWED*/
q[p-a+4] = 3;	/*SAFE /4 kod p-a, a[12] => FLAWED*/

}
