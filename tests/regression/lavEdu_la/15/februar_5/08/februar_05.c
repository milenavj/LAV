#include <stdio.h>

main()
{
	int n,i,j = 1;

	scanf("%d",&n);
	
	if(n < 1) printf ("-1");
	else
	{
	while(n >= j)
	{
		for(i = j; i <= n; i++) printf("%u ",i);
		j++;
		n -= 1;
	}
	}
	
	

 	return 0;
}
