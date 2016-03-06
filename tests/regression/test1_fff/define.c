/* define.c */

#define MAX 15
#define ZERO 0
#define MIN 1


main()
{
int a, b, c;
a = MAX;
b = ZERO;
c = (a - b)/(MAX - MIN); /*SAFE*/
if(c > 0)
	c = (a/(a - 15*MIN)); /*FLAWED*/
else 
	c = (a/(MIN+1)); /*UNREACHABLE*/

}