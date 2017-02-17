#include <stdio.h>

int check_main(int br1, int br2, int br3) {

  int a[10], i, max; //znam da je komplikovano, ali radi

  //  for(i=0;i<3;i++)
  //  {
  //  scanf("%d",&a[i]);
  //  }
  a[0] = br1;
  a[1] = br2;
  a[2] = br3;

  max = a[0];
  for (i = 0; i < 3; i++) {

    if (a[i] > max)
      max = a[i];

  }

  //  printf("Najveci je %d\n",max);
  return max;

}
