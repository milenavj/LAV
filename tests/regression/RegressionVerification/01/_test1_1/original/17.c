#include <stdio.h>
int main() {
  int a, b, c, d, e; /* deklaracija promenljivih*/
  printf("Unesi pozitivan cetvorocifreni broj:");
  scanf("%d", &a);      /* unos cetvorocifrenog broja*/
  if (a > 999 &&
      a < 10000) {      /* uslov, kojim izdvajamo samo cetvorocifrene brojeve */
    b = a % 10;         /* izdvajanje cifre hiljade */
    c = (a / 10) % 10;  /* izdvajanje cifre stotine*/
    d = (a / 100) % 10; /* izdvajanje cifre desetice*/
    e = a / 1000;       /* izdvajanje cifre jedinice*/
    if (b > c && c > d && d > e)
      printf("1\n"); /* uslov za opadajuci poredak*/
    else if (b < c && c < d && d < e)
      printf("2\n"); /* uslov za rastuci poredak*/
    else
      printf("3\n"); /* za ne uredjene */
  } else
    printf("-1\n"); /* ispis ukoliko broj nije cetvorocifren*/

  return 0;
}
