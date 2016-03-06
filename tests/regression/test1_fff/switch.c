/* switch.c */
main()
{
    int a, b = 5;
switch(a)
{
case 1: a = b/a; break; /*SAFE*/ 
case 2: a = b/a + 1; break; /*SAFE*/
case 3: a++; break;  
default: a = 5; 
}

b = b/a; /*SAFE*/
b = b/(a - 5); /*UNSAFE*/
}