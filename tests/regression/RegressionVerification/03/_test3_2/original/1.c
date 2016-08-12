#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define max 50
void izmeni(char s[]) {
  int i;
  for (i = 0; i < strlen(s); i++) {
    if (i % 3 == 0)
      s[i] = toupper(
          s[i]); //provera da li index daje ostatak nula pri deljenju sa 3
    if (i % 3 == 1)
      s[i] = tolower(
          s[i]); //Provera da li index daje ostatak jedan pri deljenju sa 1
  }
}
int main() {
  char s[max];
  printf("Unesite rec: "); //Unos reci
  scanf("%s", s);
  izmeni(s);          //Obrada reci
  printf("%s \n", s); //Ispis reci
  return 0;
}
