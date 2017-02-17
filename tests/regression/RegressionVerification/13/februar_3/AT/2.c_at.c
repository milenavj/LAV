
#include <stdio.h>
#include <math.h>

int check_main(double a, double b, double ret_array[]) {

  double t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, x;

  // scanf("%lf%lf", &a, &b);
  if (a >= b) {
    {
      printf("-1\n");
      ret_array[0] = -1;
    }
    return -1;
  }
  x = (b - a) / 9;
  t1 = a;
  t2 = t1 + x;
  t3 = t2 + x;
  t4 = t3 + x;
  t5 = t4 + x;
  t6 = t5 + x;
  t7 = t6 + x;
  t8 = t7 + x;
  t9 = t8 + x;
  t10 = t9 + x;

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
                      printf("%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
                             cos(t1), cos(t2), cos(t3), cos(t4), cos(t5),
                             cos(t6), cos(t7), cos(t8), cos(t9), cos(t10));
                      ret_array[0] = cos(t1);
                    }
                    ret_array[1] = cos(t2);
                  }
                  ret_array[2] = cos(t3);
                }
                ret_array[3] = cos(t4);
              }
              ret_array[4] = cos(t5);
            }
            ret_array[5] = cos(t6);
          }
          ret_array[6] = cos(t7);
        }
        ret_array[7] = cos(t8);
      }
      ret_array[8] = cos(t9);
    }
    ret_array[9] = cos(t10);
  }

  return 0;
}

#include "../check.h"
