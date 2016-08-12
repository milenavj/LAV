
#include <stdio.h>
#include <math.h>

int check_main(double a, double b, double ret_array[]) {
  int i;
  double c;
  // scanf("%lf%lf",&a,&b);
  if (a < b) {
    c = (b - a) / 9;
    {
      {
        {
          {
            {
              {
                {
                  {
                    {
                      {
                        printf("%.4lf %.4lf %.4lf %.4lf %.4lf %.4lf %.4lf "
                               "%.4lf %.4lf %.4lf\n",
                               cos(a), cos(a + c), cos(a + 2 * c),
                               cos(a + 3 * c), cos(a + 4 * c), cos(a + 5 * c),
                               cos(a + 6 * c), cos(a + 7 * c), cos(a + 8 * c),
                               cos(a + 9 * c));
                        ret_array[0] = cos(a);
                      }
                      ret_array[1] = cos(a + c);
                    }
                    ret_array[2] = cos(a + 2 * c);
                  }
                  ret_array[3] = cos(a + 3 * c);
                }
                ret_array[4] = cos(a + 4 * c);
              }
              ret_array[5] = cos(a + 5 * c);
            }
            ret_array[6] = cos(a + 6 * c);
          }
          ret_array[7] = cos(a + 7 * c);
        }
        ret_array[8] = cos(a + 8 * c);
      }
      ret_array[9] = cos(a + 9 * c);
    }

  } else {
    printf("%d\n", -1);
    ret_array[0] = -1;
  }
  // return 0;
}

#include "../check.h"
