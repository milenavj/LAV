
#include<stdio.h>
main()
{
long int a, x1, x2, x3, x4, k;
printf("Uneti cetvorocifren broj : ");
scanf("%d", &a);
if(a<0)
printf("Broj je negativan : %d\n", -1);
else if(a<=999 || a>9999)
printf("Broj nije cetvorocifren : %d\n", -1);
else{
x1=a%10;
x2=(a/10)%10;
x3=(a/100)%10;
x4=a/1000;
if((x1%2==0) || (x2%2==0) || (x3%2==0) || (x4%2==0)){
k=1;
if(x1%2==0)
k=k*x1;
if(x2%2==0)
k=k*x2;
if(x3%2==0)
k=k*x3;
if(x1%2==0)
k=k*x4;
printf("Prozivod parnih cifara je : %d\n", k);
}
else
printf("Broj nema parnih cifara : %d\n", 1);
}
return 0;
}