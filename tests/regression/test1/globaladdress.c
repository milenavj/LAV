int x;

int main(){
int* p, a;
a = x;
p = &x;
*p = 1; /*SAFE*/
*p = *p/a; /*SAFE, FLAWED, SAFE*/
}

