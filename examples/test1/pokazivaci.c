

main()
{
int* p;
int i = 5;
int j = 2;
int a[10];

p = a;
p[i] = 3; /*SAFE*/
a[0] = p[i]; /*SAFE, SAFE */

a[p[0]+7] = 3;/*SAFE, FLAWED*/

if(i>3) i++;
else i--;

if(j>3) j++;
else j--;

if(j>3) j++;
else j--;

if(j>3) j++;
else j--;

if(j>3) j++;
else j--;


i = p[--i] ;	/*SAFE*/
p[i] = 5; 	/*SAFE */
p[12] = 7; 	/*FLAWED*/
p[j] = 7; 	/*FLAWED*/
j = p[j]; 	/*FLAWED*/
/*j = p[a[0]];
p[j] = 3;*/
}










/*int a, b, *q, *p, s = 1;
a = 1;
b = a; 
p = &a;
q = p;
*p = 2;
*q = 0;
s = s/a;
*/
/*int a, b, *p, *q, s = 1;
a = 0;
b = a+2;
p = &a;
q = &b;
*p = 2;
*q = 0;
s = s/a;
s = s/b;
*/
