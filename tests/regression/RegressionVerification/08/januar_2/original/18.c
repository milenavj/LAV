#include <stdio.h>

int strcspn(char s[], char t[]) {
  int i, j;
  for (i = 0; s[i]; i++) //ide redom kroz nisku s
      {
    for (j = 0; t[j]; j++) //sada uporedjuje redom sa karakterima niske t
      if (s[i] == t[j])    //ako nadje jedan...
        return i;          //vraca trenutnu poziciju u niski s kao resenje
    //dolazak dovde znaci da trenutni karakter niske s nije u niski t
  }
  //do ovde se dolazi samo ako nijedan karakter iz s nije sadrzan u t
  return -1; //u tom slucaju je -1 resenje
}

#if 0
void unos_niske(char s[]) //pomocna f-ja za unos niske do novog reda
    {
  int i = 0;
  char c = getchar();
  while (c != '\n' && c != '\r' && c != EOF) {
    s[i++] = c;
    c = getchar();
  }
  s[i] = 0; //ne sme se zaboraviti null-terminator!
}
//gets u string ubacuje i \n sto nije dobro, a scanf("%s") cita do bilo
//koje praznine, a ne samo do kraja reda. zato sam napisao ovu f-ju.
#endif

int main() {
  char s[21], t[21]; //20 (max) karaktera + null za kraj stringa

  unos_niske(s);
  unos_niske(t);

  //fprintf(stderr, "%s\n", s);
  //fprintf(stderr, "%s\n", t);

  printf("%d\n", strcspn(s, t)); //poziv f-je

  return 0;
}
