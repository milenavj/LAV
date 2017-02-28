#include "stdio.h"
#include "string.h"
#define SIZE 21
int izbacuje(char rec[], int c) {
  int i, j = 0;
  if (c < 0 || c > 20 ||
      c + 1 > strlen(rec)) // Uslov za pozicuju da li je uneta pozicija koju
                           // niska nema....
    return 0;
  for (i = 0; i < strlen(rec);
       i++, j++) // Prolazimo kroz nisku i izbacujemo slovo iz niske...
      {
    if (i == c) {
      rec[j] = rec[i + 1];
      i++;
    } else
      rec[j] = rec[i];
  }
  rec[j] = '\0';
}
int main() {
  char rec[SIZE], rec1[SIZE]; // Inicijalizacija Niski
  int c, m;

  printf("Unesite poziciju koju zelite da izbacite: "); // Unos pozicije koju
                                                        // zelite da izbacite
  scanf("%d", &c);
  printf("Unesite rec: "); // Unos niske
  scanf("%s", rec);
  m = izbacuje(rec, c);
  if (m) {
    printf("1 \n");
    printf("%s \n", rec);
  } else
    printf("-1\n");

  return 0;
}
