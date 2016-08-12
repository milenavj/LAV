#include <stdio.h>
#include <stdlib.h>
int main() {
  unsigned int sat, minut, sekund, c, preostalo, novi, a, b;
  scanf("%d%d%d", &sat, &minut, &sekund);

  if (sat < 0 || sat > 23) {
    printf("-1\n");
    exit(1);
  }
  if (minut < 0 || minut > 59) {
    printf("-1\n");
    exit(1);
  }
  if (sekund < 0 || sekund > 59) {
    printf("-1\n");
    exit(1);
  }

  c = 12 * 3600;

  preostalo = ((sat * 3600) + (minut * 60) + sekund);
  novi = 23 * 3600 + 59 * 60 + 60;
  if (sat >= 12) {
    a = novi - (preostalo - c);
    printf("%d\n", a);
  } else {
    b = (c - preostalo);
    printf("%d\n", b);
  }

  return 0;
}
