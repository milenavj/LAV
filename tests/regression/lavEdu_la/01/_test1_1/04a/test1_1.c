 
#include<stdio.h>
int main ()
{
	int x, h, s, d, j;
	printf("Unesi pozitivan cetvorocifren broj: \n");
	scanf("%d", &x);

	h=x/1000;
	s=(x/100)%10;
	d=(x/10)%10;
	j=x%10;
	
	if ((x<1000) || (x>9999))
	printf("%d\n", -1);
		else
			{
			if ((h>s) && (s>d) && (d>j))
			printf("%d\n", 2);

			else 
			if ((j>d) && (d>s) && (s>h))
			printf("%d\n", 1);

			else 
			printf("%d\n", 3);
			}

	return 0;

}


