
int a [100];


void g(int * x, int y){
   x[y]=0;
}


int main(){

   g(a,0);

   return 1;   
}
