void f()
{
int a = 0;
a++;
}

void g()
{
int a = 0;
a++;
return;
}

int main()
{
int a = 0;
f();
a++;
g();
a = a/(a-1);
return 0;
}