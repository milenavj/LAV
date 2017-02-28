
#include <stdio.h>

int check_main(unsigned int sat, unsigned int minut, unsigned int sekund) {

  unsigned nar_pop;
  // scanf ("%u%u%u", &sat, &minut, &sekund);

  if (sat < 0 || sat > 23 || minut < 0 || minut > 59 || sekund < 0 ||
      sekund > 59) {
    {
      printf("-1\n");
      return -1;
    }
    // return -1;
  }

  if (sat < 12) {
    nar_pop = (12 - sat) * 3600 - minut * 60 - sekund;
    {
      printf("%u\n", nar_pop);
      return nar_pop;
    }
    // return 0;
  } else {
    nar_pop = (24 - sat) * 3600 - minut * 60 - sekund;
    {
      printf("%d\n", 12 * 3600 + nar_pop);
      return 12 * 3600 + nar_pop;
    }
    // return 0;
  }

  // return 0;
}

#include "../check.h"
