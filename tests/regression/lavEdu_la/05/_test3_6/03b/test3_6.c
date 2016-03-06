#include <stdio.h>
#define MAX 20
#include <string.h>

int ubaci (char t[], int k, int n)
{
int i;
if (k<0 || n<=k || n>20)
    return 0;		
else
{
    t[n+1]='\0';
    for (i=n; i>=k; i--)
        t[i+1] = t[i];
}
    
return 1; 
}


int main ()
{
int n, k, x;
char c[MAX];
printf("Unesi poziciju: ");
scanf("%d", &k);
    
printf("Unesi rec: ");
scanf("%s", &c);

n = strlen(c);
x = ubaci(c,k,n);
        
    if (x==1)
        {
        printf ("%d\n", 1);
        printf ("%s\n", c);
        }
    else 
        printf ("%d\n", -1);
        return 1;

}


