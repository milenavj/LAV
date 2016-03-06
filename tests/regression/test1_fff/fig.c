
int glob = 10;
int glob2;
int globniz[5];

int g()
{
int b;
int* p;
p = &b;
*p = 5+glob;	/*SAFE*/
return b;
}

int f()
{
int b, c;
int* p;
c = g(); 	/*c = 15 25*/
p = &b;
*p = 100/(5+c); /*SAFE, UNSAFE*/
c = g(); 	/*c = 15 25*/
*p = *p + 3 + c + glob;/*SAFE,SAFE b = 28*/
return b;
}


main()
{
int a, b, c;
int* ptr;
int niz1[5];
int niz2[7];

niz1[0] = 5; /*SAFE*/
niz2[0] = 3; /*SAFE*/


a = 10/(niz1[0]-5);  /*SAFE, FLAWED*/

if(a>0) a++;

ptr = &b;
b = f(); 	/*SAFE*/
glob = 20;
c = f(); 	/*SAFE*/

globniz[c] = 5; /*FLAWED*/

a = b/(c-51); /*FLAWED*/

}

