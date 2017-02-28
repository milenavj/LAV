int ASSERT(int); int ASSUME(int);
#include <malloc.h>
#include <stdlib.h>

typedef struct {

  int x;
  int z;
} st_t;

st_t * st_alloc(int x){
  st_t * t = (st_t *) malloc(1 * sizeof(st_t));
  if(t == 0) exit(1);
  if ( x >  0){
    t -> x = x;
    t -> z = 0;
  } else {

    t -> x = 0;
    t -> z = 5;
  }
  return t;
}

int st_compact(st_t * st1){
  if (st1 -> z > 0 ){
      ASSERT(st1 -> x > 0);
  }

  return st1 -> x;

}

int b_main(int a, int b){
  st_t * st1, * st2;
  ASSUME(a>0);
  
  st1 = st_alloc(a);
  st2 = st_alloc(-a);

  ASSUME(st1!=st2);  

  st_compact(st1);
  return 1;
}

int main(void)
{
  int a, b;
  return b_main(a,b);
}
