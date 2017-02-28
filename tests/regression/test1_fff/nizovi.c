
main()
{
int a[20];	
int b;		
int *c;		

b = 5;		
a[5] = 3;	/*SAFE*/
c = a;		
c[3] = b;	/*SAFE*/
c[b] = 4;	/*SAFE*/
c = a + 3;	
c[2] = 2;	/*SAFE*/

}
