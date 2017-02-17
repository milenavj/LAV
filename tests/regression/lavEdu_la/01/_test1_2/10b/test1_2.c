#include <stdio.h>

int main(){
  int x;
  int p = 1; //na pocetku je proizvod 1
  
  printf("Unesi jedan pozitivan cetvorocifren broj: ");
  scanf("%d", &x);
  
  if(x<1000 || x>9999){
    p = -1; //neispravan unos, mora da bude cetvorocifren
  }else{
    while(x > 0){ //dok ima jos cifara... (a bice ih tacno 4)
      if(x%2 == 0) //paran je broj, dakle parna je i poslednja cifra
        p *= (x%10); //pomnozi p sa vrednoscu cifre...
      x /= 10; //sada podeli x sa 10 kako bi se proverila sledeca cifra...
    }
    //ako nije bilo nijedne parne cifre, rezultat (p) ce ostati 1
    //kao na pocetku, jer nijednom nije pomnozen s necim
  }
  
  printf("Rezultat je: %d\n", p);
  return 0;
}
