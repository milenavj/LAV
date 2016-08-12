
#include <stdio.h>
int check_main(int a) {
  int b, c, d, e; /* deklaracija promenljivih */
  printf("Unesi pozitivan cetvorocifreni broj:");
  // scanf ("%d", &a);          /* unos cetvorocifrenog broja */
  if (a > 999 &&
      a < 10000) {      /* uslov, kojim izdvajamo samo cetvorocifrene brojeve */
    b = a % 10;         /* izdvajanje cifre hiljade */
    c = (a / 10) % 10;  /* izdvajanje cifre stotine */
    d = (a / 100) % 10; /* izdvajanje cifre desetice */
    e = a / 1000;       /* izdvajanje cifre jedinice */
    if (b > c && c > d && d > e) {
      printf("1\n");
      return 1;
    } /* uslov za opadajuci poredak */
        else if (b < c && c < d && d < e) {
      printf("2\n");
      return 2;
    } /* uslov za rastuci poredak */
        else {
      printf("3\n");
      return 3;
    } /* za ne uredjene */
  } else {
    printf("-1\n");
    return -1;
  } /* ispis ukoliko broj nije cetvorocifren */

  // return 0;
}

#include "../check.h"
