#include <stdio.h>
#define BROJELEMENATA 100
/* simbolicka konstanta za maximalan broj elemenata niza */

float MAX(float a[],int n);

float AS(float a[],int n); /* deklaracije funkcija */



float MAX(float a[],int n){
 int i;
 float max; 
   max=a[0];  /*maximum postavljen kao prvi element */
  for(i=1;i<n;i++){
    if(a[i]>max){
      max=a[i]; /* ukoliko je neki element veci od maximuma on postaje maximum */
                }
                    }
return max; 
}

float AS(float a[],int n){
 float zb=0,ars;
 int i;
  for(i=0;i<n;i++){
    zb=zb+a[i]; /* zbir elemenata */
                   }

ars=(float)zb/n; /* racunanje aritmeticke sredine */

return ars;

}


int main(){
 int n,i;
 float a[BROJELEMENATA],j,k;
   printf("Uneti dimenziju niza: ");
   scanf("%d",&n);
if(n<=0 || n>BROJELEMENATA){ /* ispitivanje da li je dobro unesen broj elemenata */
   printf("Izlaz: \n -1\n"); /* ukoliko br. elemenata nije dobro unesen ispis -1 */
                            }
else{
  printf("Uneti niz: "); 
   for(i=0;i<n;i++)
    scanf("%f",&a[i]); /* unos niza */
  printf("Izlaz: \n"); 
  printf("%f\n",MAX(a,n)); /* ispis maximuma */
  printf("%f\n",AS(a,n)); /* ispis ar. sredine */
  j=MAX(a,n);
  k=AS(a,n)*2; 
   if(j>k) /* ispitivanje da li je maximum dvostruko strogo veci od ar. sredine */
    printf("1\n");
   else
    printf("0\n");
}

return 0;
}
