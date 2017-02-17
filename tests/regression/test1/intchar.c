int assert(int);
int main()
{
int a = 5;
char c = 1;

assert(a==c);
assert(a==1);
assert(1==c);
assert(1==2);

assume(a == c);
assert(1==c);


}
