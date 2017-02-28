
#include <malloc.h>
#include <stdlib.h>
int assert(int); 
void all(){
  int * st1, * st2;

  st1 = malloc(4);  
  st2 = malloc(4);
  if((st1==0) || (st2==0)) exit(1);
  assert(st1!=st2);  
}

int main()
{
  all();
}
