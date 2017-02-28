
#include <stdio.h>

int check_main(int br1, int br2, int br3) {

  int max;

  //printf("unesite 3 cela broja\n");
  // scanf ("%d%d%d", &br1, &br2, &br3);
  max =
      (br1 >= br2 && br1 >= br3) ? br1 : (br2 >= br1 && br2 >= br3) ? br2 : br3;

  {
    printf("%d\n", max);
    return max;
  }
  // return 0;
}

#include "../check.h"
