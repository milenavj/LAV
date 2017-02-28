
#include <stdio.h>

int check_main(unsigned int sat, unsigned int minut, unsigned int sekund) {
  unsigned int dan, ostalo, vreme, podne;
  // scanf ("%d%d%d", &sat, &minut, &sekund);
  if (sat < 0 || sat >= 24 || minut < 0 || minut > 59 || sekund < 0 ||
      sekund > 59) {
    {
      printf("-1\n");
      return -1;
    }
    // return -1;
  }
  dan = 24 * 3600;
  vreme = sat * 3600 + minut * 60 + sekund;
  /*if(sat==12 && minut==0 && sekund==0) printf("%d\n")*/

  podne = 12 * 3600;
  if (sat < 12) {
    ostalo = podne - vreme;
    {
      printf("%d\n", ostalo);
      return ostalo;
    }
    // return 0;
  }
  if (vreme > podne) {
    ostalo = dan - vreme + podne;
    {
      printf("%d\n", ostalo);
      return ostalo;
    }
    // return 0;
  }
  if (vreme == podne) {
    {
      printf("%d\n", dan);
      return dan;
    }
    // return 0;
  }

  // return 0;
}

#include "../check.h"
