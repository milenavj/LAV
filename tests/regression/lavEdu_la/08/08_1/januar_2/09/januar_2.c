#include <stdio.h>
#include <string.h>

int strcspn1(char s[], char t[])
{
	int i, j;
	for(i=0; s[i]; i++)
		{
			for(j=0; t[j]; j++)
				if(s[i] == t[j])
					return i;
		}
	
	return -1;
}

int main()
{
	char s[20], t[20];
	int i, n, m;
	scanf("%s", s);
	scanf("%s", t);

	
	i = strcspn1(s, t);
	if(i == -1)
		printf("-1\n");
	else
		printf("%d\n", i);
	
	return 0;
}

