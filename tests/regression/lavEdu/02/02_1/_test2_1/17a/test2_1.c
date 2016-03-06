 
#include<stdio.h>

#define MAXEL 100
//max - max clan niza a
float max_broj(float a[], int n)
{
  int i;
  float max; 
  max=a[0];
  for (i=1;i<n;i++)
  {
    if (a[i]>max)
      max=a[i];
  }
  return max;
} 
//rez - rezultat u koji ce biti upisana aritmeticka sredina niza
float aritmeticka_sredina(float a[], int n)
{
  int i;
  float rez=0;
  for (i=0;i<n;i++)
    rez=rez+a[i];
  return (rez/n);
}

//ar - aritmeticka sredina niza, max - max niza, a - niz, n - broj elemenata niza
int main()
{
  float a[MAXEL],max,ar;
  int i,n;
  printf("Unesite broj elemenata niza\n");
  scanf("%d",&n);
  if (n<=0)
    printf("-1\n");
  else
  { 
    printf("Unesite elemente niza\n");
    for (i=0;i<n;i++)
    scanf("%f",&a[i]);
    max=max_broj(a,n);
    ar=aritmeticka_sredina(a,n);
    printf("Max element niza je %f\n",max);
    printf("Aritmeticka sredina niza je %f\n",ar);
    if (max>2*ar)
      printf("1\n");
    else
      printf("0\n");
  }
}