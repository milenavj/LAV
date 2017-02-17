
#include <stdio.h>

int check_main(unsigned int sat, unsigned int minut, unsigned int sekund) {
  unsigned int vreme, podne, pom, max;
  // scanf ("%d%d%d", &sat, &minut, &sekund);
  podne = 12 * 3600;
  max = 23 * 3600 + 59 * 60 + 60;
  vreme = sat * 3600 + minut * 60 + sekund;
  if ((sat >= 0 && sat < 24) && (minut >= 0 && minut < 60) &&
      (sekund >= 0 && sekund < 60)) {
    if (vreme <= podne) {
      pom = podne - vreme;
    }
    if (vreme > podne) {
      pom = podne + (max - vreme);
    }
    {
      printf("%d\n", pom);
      return pom;
    }
  } else {
    printf("-1\n");
    return -1;
  }

  // return 0;
}

#include "../check.h"
