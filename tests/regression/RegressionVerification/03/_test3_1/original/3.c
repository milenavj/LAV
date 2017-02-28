#include <stdio.h>
#include <string.h> /*ubacujemo biblioteku string.h zbog %s u scanf*/
#define size 21

void izmeni(
    char a[]) /*pravimo izmeni, on je tipa void, pa nema return, u njemu vrtimo
jedan for ciklus koji cita elemente niske a, proverava za parna mesta da li
su mala slova kako bi ih pretvorio u velika, a za neparna mesta radi obrnuto. Za
ostale znake ne radi nista.*/
    {
  int i;
  for (i = 0; a[i]; i++) {
    if (i % 2 == 0)                   /*provera da li je parno mesto*/
      if (a[i] <= 'z' && a[i] >= 'a') /*provera da li je slovo malo*/
        a[i] = a[i] - 'a' + 'A';      /*pretvaranje u veliko*/

    if (i % 2 != 0)                   /*provera da li je neparno mesto*/
      if (a[i] <= 'Z' && a[i] >= 'A') /*provera da li je slovo veliko*/
        a[i] = a[i] - 'A' + 'a';
  } /*pretvaranje u malo*/
}

int main() {
  char a[size]; /*deklarisemo niz i postavljamo ga na podrazumevanu duzinu
 definisanu u makrou*/
  printf("Unesi rec: ");
  scanf("%s", a);    /*unosimo rec preko standardnog ulaza*/
  izmeni(a);         /*vrsimo funkciju izmeni sa niskom a kao argument*/
  printf("%s\n", a); /*vrsimo ispis na standardni izlaz*/
  return 0;
}
