#include <stdio.h>

int main()
{
    int broj, cif1, cif2, cif3, cif4;
    printf("Unesi jedan pozitivan cetvorocifren broj: \n");
    scanf("%d", &broj);
    if((broj < 0)|| (broj < 1000) || (broj > 9999)) printf("-1\n");
        else{
              cif1 = broj / 1000; 
              cif2 = (broj / 100) % 10;
              cif3 = (broj / 10) % 10; 
              cif4 = broj % 10; 
     if ((cif1 < cif2) && (cif2 < cif3) && (cif3 < cif4)) printf("1\n");
     else if ((cif1 > cif2) && (cif2 > cif3) && (cif3 > cif4)) printf("2\n");
     else printf("3\n"); 
     


}
    return 0;
}


