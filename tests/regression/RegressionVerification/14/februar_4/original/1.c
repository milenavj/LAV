#include <stdio.h>
#include <stdlib.h>

int main() {
  unsigned int sat, minut, sekund;
  scanf("%u %u %u", &sat, &minut, &sekund);
  if (sat < 0 || sat > 23 || minut < 0 || minut > 59 || sekund < 0 ||
      sekund > 59) {
    printf("-1\n");
    return 1;
  }
  unsigned int k;
  if (sat == 12) {
    k = 23 * 3600 + (59 - minut) * 60 + (60 - sekund);
    printf("%u\n", k);
    return 0;
  }
  if (sat > 12)
    if (minut > 0 || sekund > 0)
      k = ((24 - abs(12 - sat)) - 1) * 3600 + (59 - minut) * 60 + (60 - sekund);
    else
      k = (24 - abs(12 - sat)) * 3600;
  else if (minut > 0 || sekund > 0)
    k = ((12 - sat) - 1) * 3600 + (59 - minut) * 60 + (60 - sekund);
  else
    k = (12 - sat) * 3600;
  printf("%u\n", k);
  return 0;
}
