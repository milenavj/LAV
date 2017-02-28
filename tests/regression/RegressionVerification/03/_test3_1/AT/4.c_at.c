
#include <stdio.h>
#define MAX 21

void izmeni(char rec[]) {
  int i;
  for (i = 0; rec[i]; i++) //Prolazimo kroz celu nisku do terminalne nule
    if (i % 2 == 0) //na svakoj parnoj pozici niske, proveravamo da li je slovo
                    //malo, ako jeste menjamo ga u veliko
        {
      if (rec[i] >= 'a' &&
          rec[i] <= 'z')             //proveravamo da li je uneti znak slovo
        rec[i] = rec[i] - 'a' + 'A'; //menjamo malo u veliko slovo
    } else //na svakoj neparnoj poziciji niske, proveravamo da li je slovo
           //veliko, ako jeste menjamo ga u malo
        if (rec[i] >= 'A' &&
            rec[i] <= 'Z')           //proveravamo da li je uneti znak slovo
      rec[i] = rec[i] - 'A' + 'a';   //menjamo veliko u malo slovo
}

#if 0
int main() {
  char rec[MAX]; //deklaracija niza
  printf("Unesi rec: ");
  scanf("%s", rec);    //Unos reci
  izmeni(rec);         //Pozivanje funkcije
  printf("%s\n", rec); //Stampanje reci posle izvrsavanja funkcije

  return 0;
}
#endif
#include "check.h"
