
struct tacka
{
int x;
};

main()
{
struct tacka t, t1;

t.x = 3;
t1.x = 5;

t.x = 0;

t.x = t.x/t1.x;

}