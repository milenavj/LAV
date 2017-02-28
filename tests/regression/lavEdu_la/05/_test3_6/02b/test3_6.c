#include<stdio.h>
#include<string.h>

#define MAX 21

// Funkcija za ubacivanje slova na datu poziciju k u stringu s
int ubaci(char s[], int n, int k)
{
	int i;
	if (n>20 || n<=k || k<0)
		return 0;
	else
	{
		s[n+1]='\0';
		for (i=n;i>=k;i--)
			s[i+1]=s[i];
		return 1;
	}
}


// s-string, k-pozicija znaka koji se duplira, n-duzina reci 
//m - vrednost koju vraca funkcija ubaci
int main()
{
	char s[MAX];
	int k,n,m;
	printf("Unesite poziciju slova za dupliranje.\n");
	scanf("%d",&k);
	printf("Unesite rec.\n");
	scanf("%s", s);
	n=strlen(s);
	m=ubaci(s,n,k);
	if (m==1)
	{
		printf("%d\n",1);
		printf("%s\n",s);
	}
	else
		printf("%d\n",-1);
	return 1;
}
