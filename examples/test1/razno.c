/*int funkcija(int a)
{
if(a>10) return 5;
else if (a>5) return 4;
else if (a>0) return 3;
return 1;
}*/

main()
{
int a, *b, *p, k;
a = 5;
b = &a;
*b = 1;	  /*SAFE */
b[0] = 1; /*SAFE */
b[1] = 2;  /*FLAWED */
if(a>3) *b = *b/a; /*UNREACHABLE, UNREACHABLE, UNREACHABLE*/

k = 0;
a = k;
p = b;
p[3] = 2;	/*FLAWED*/
*b = *b/(*p); /*FLAWED SAFE SAFE SAFE*/

p = p+3;  
*p = 3;  /*FLAWED */


a = 3/a;  /*SAFE*/

p = b;

/*int a=2, k, b=1, c=0;

for(;;);

if(a>0)
{
	k = 0;
	if(b>0) k++;
	else {
		if (k==0) k--;
		else k++;
		}
}
else a--;
*/
}
