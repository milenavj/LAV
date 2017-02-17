#include<stdio.h>

#define MAX 20 

int strcspn(char st[], char t[])
{
	int i, j;

	for(i=0; st[i]; i++)
	{
		for(j=0; t[j]; j++)
		{
			if(st[i]==t[j])
			return i;
		}
	}
	return -1;

}
int main()
{
	char s[MAX];
	char t[MAX];
	int a;
	
	printf("Ucitaj prvu nisku ");
	scanf("%s", &s);
	printf("Ucitaj drugu nisku ");
	scanf("%s", &t);
	

	a=strcspn(s, t);
	if(a==-1)
	printf("%d\n", a);
	else
	printf("%d\n", a);

	

	

	return 0;
}
