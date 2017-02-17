
#include <stdio.h>

int check_main(unsigned int sat, unsigned int minut, unsigned int sekund) {

  long int ps;

  // scanf ("%u %u %u", &sat, &minut, &sekund);

  if (sat > 23 || minut > 59 || sekund > 59) {
    {
      printf("-1");
      return -1;
    }
  } else {

    if (sat == 12) {
      sat = 23 * 3600;

    } else if (sat > 12) {
      sat = (11 + 24 - sat) * 3600;

    } else
      sat = (11 - sat) * 3600;

    if (minut == 0) {
      minut = 59 * 60;
    } else
      minut = (59 - minut) * 60;

    sekund = 60 - sekund;

    ps = sat + minut + sekund;

    {
      printf("%ld", ps);
      return ps;
    }
  }

  // return 0;

}

#include "../check.h"
