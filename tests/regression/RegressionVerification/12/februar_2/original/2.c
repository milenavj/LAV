#include <stdio.h>

int main() {

  int br1, br2, br3, max;

  //printf("unesite 3 cela broja\n");
  scanf("%d%d%d", &br1, &br2, &br3);
  max =
      (br1 >= br2 && br1 >= br3) ? br1 : (br2 >= br1 && br2 >= br3) ? br2 : br3;

  printf("%d\n", max);
  return 0;
}
