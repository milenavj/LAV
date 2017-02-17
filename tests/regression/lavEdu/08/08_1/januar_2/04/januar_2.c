#include<stdio.h>
#define maxNiza 21
int strcspn1(char s[], char t[])
{
	int i, j;
	for(i=0;s[i];i++)
		for(j=0;t[j];j++)
			if(s[i]==t[j])
				return i;
	return -1;
}
int main()
{
	int rezultat;
	char s[maxNiza], t[maxNiza];
	scanf("%s", s);
	scanf("%s", t);
	rezultat = strcspn1(s, t);
	if(rezultat!=-1)
		printf("%d\n", rezultat);
	else
		printf("%d\n", rezultat);
	
	return 0;
}
