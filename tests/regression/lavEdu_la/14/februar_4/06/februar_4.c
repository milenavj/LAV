#include <stdio.h>
#include <stdlib.h>

int main(){

unsigned int sat,minut,sekund,sat1=0,min1=0,sek1=0,t1,t2,t3,podne;

printf("Ulaz: "); scanf("%d%d%d",&sat,&minut,&sekund);
if (sat>=24 || minut>=60 || sekund>=60)
{ printf("Izlaz: -1\n"); exit(1); }

t1=86400;
podne=t1/2;
sat1=(sat*60)*60;
min1=minut*60;
sek1=sekund;
t2=sat1+min1+sek1;

if (t2<podne)
t3=podne-t2;
else if (t2>=podne)
t3=t1-t2+podne;

printf("Izlaz: %d\n",t3);

return 0;
} 
