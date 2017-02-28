/* global.c */

int a;
int b;
const int c = 3;
int k = 5;
int niz[4];
char *ptr = "ana";
main()
{
    b = a / k;			/*SAFE*/
    niz[a] = 3; 		/*SAFE*/
    b = a/k;			/*SAFE*/
    a = 5;
    niz[a-2] = 3;		/*SAFE*/
    niz[a-7] = 3;		/*FLAWED*/

    a = 0;
    a++;
    a = a / c; 			/*SAFE*/
    a++;
    if(a > 0) a = 100/a;	/*SAFE*/
    b = a/b;   			/*FLAWED*/ 
    b = b/(c - 3); 		/*FLAWED*/
    if(a>0)
    { 
	int b = 3;
	b++;
	a = a/b;		/*SAFE*/
	a = a/(b - 4);		/*FLAWED*/ 
    }
    b = a/k;  			/*SAFE*/  
    niz[2] = 5; 		/*SAFE*/
    niz[c] = 5; 		/*SAFE*/
    a = c-1;
    niz[a] = 5; 		/*SAFE*/
 
   niz[niz[2]] = 4;		/*SAFE, FLAWED*/ 
   niz[10] =5; 			/*FLAWED */
}