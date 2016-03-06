

main()
{
int a[10];

a[0] = 3; /*SAFE*/
a[a[0]+7] = 3;/*SAFE,FLAWED*/

}



