#include <stdio.h>
#include <string.h>

void podeli(int k, int l)
{
int i, j, n, r=0;

if(r>0) 
	r++;

for(i=0; i<3;i++)
	for(j=0; j<3; j++)
		r+=i+j;
if(r>0) 
	r++;
r = k/l;
k++;
l++;
r++;
}

int main()
{

int k=5, l=3, i, j;

if(i>j) 
	i++;
if(i>j) 
	i++;
if(i>j) 
	i++;

if(k>l) 
	l--;
else 
	l++;
if(k>l) 
	l--;
else 	
	l++;
if(k>l) 
	l--;
else 
	l++;

if(i<=j) 
	i++;

podeli(k, l);

return 0;
} 
	
