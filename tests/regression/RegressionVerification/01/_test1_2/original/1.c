#include <stdio.h>
int main() {
  int n, x, y = 1; /* Ovde deklarisem promenjive. Stavio sam da je y=1 zbog
mnozenja (u suprotnom rezultat bi uvek bio 0). */
  printf("Unesite pozitivan cetvorocifren broj: ");
      /* Stampam poruku da treba da se unese broj */
  scanf("%d", &n); /* Ucitavam broj */

  if ((n < 1000) ||
      (n > 9999))   /* Proveravam da li je broj cetvorocifren i pozitivan */
    printf("-1\n"); /* Ako nije, ispisujem -1 */
  else {            /* Ako jeste, krecem sa petljom */
    while (n != 0) {
      x = n % 10; /* Uzimam poslednju cifru tj. ostatak pri deljenju sa 10 */
      if (x % 2 == 0)
        y = y * x;
            /* Proveravam da li je parna i ako jeste mnozim je sa rezultatom */
      n = n / 10; /* Ovde "brisem" broj koji sam proverio da bi mogao da uzmem
 sledecu cifru kad opet udjem u petlju */
    } /* Posto se n stalno deli sa 10, jednom ce sigurno biti 0. Na taj nacin
sam obezbedio izlazak iz petlje. */

    printf("%d\n", y); /* Ovde stampam resenje */
  }                    /* Ovde se zavrsava Else-blok */

  return 0;
} /* Ovde se zavrsava program */
