#include<stdio.h>
#include<math.h>

int main()
{
	int n, h, s, d, j;
	
	printf("Unesi jedan pozitivan cetvorocifren broj\n");
	scanf("%d", &n);
	if(n>9999 || n<1000) printf("-1\n");
	else
	{
		h = n / 1000;
		s = (n%1000) / 100;
		d = ((n%1000)%100)/10;
		j = ((n%1000)%100)%10;
		
		if(j>d && d>s && s>h) 
			printf("1\n");
		else
		{
			if(h>s && s>d && d>j) printf("2\n");
			else printf("3\n");
		}
	}
	return 0;
} 
