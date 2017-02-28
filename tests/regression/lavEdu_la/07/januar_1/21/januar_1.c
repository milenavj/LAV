#include <stdio.h>

int main()
	{
	float a, b;
	int max;

	scanf("%f %f", &a, &b);

	max = (a>b) ? 1 : 2;

	printf("%d\n", max);

	return 0;
	}

