#include <stdio.h>

int main()
{
	int x, p, h, s, d, j;
	printf("uneti pozitivan cetvorocifren broj:\n");
	scanf("%i", &x);
	if(x<1000 || x>9999)
	printf("rezultat je: -1\n");
	else 
	{
	h=x/1000;
	s=(x/100)%10;
	d=(x/10)%10;
	j=x%10;
	if (h%2==0);
	else h=1;
	if (s%2==0);
	else s=1;
	if (d%2==0);
	else d=1;
	if (j%2==0);
	else j=1;
	p=h*s*d*j;
	printf("rezultat je: %i\n", p);
	}
	return 0;
}
