int ASSERT(int);
int b_main(int i, int j){

   int x,y;
   int a[1];
   x=i;
   y=j;

   while (x!=0) {
      x--;
      y--;
   }

   if (i==j){
      ASSERT( y == 0);

      
   }
   
   
   return 1;
}

int main(void)
{
  int i,j;
  return b_main(i,j);
}
