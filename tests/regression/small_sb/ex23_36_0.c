
int x[4608];

int b_main(unsigned int y){
   
   unsigned int counter=0U,z;
   if ( 127 < y) return 0;
   if ( y < 0) return 0;
   z = y * 36U;
   
   while (counter < 36U){
      x[z] =0;
      z++;
      counter++;
   }

   return 1;

}

int main(void)
{
  int y;
  return b_main(y);
}
