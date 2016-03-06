#include <stdlib.h>

struct tacka
{
int x;
int y;
};

main()
{
struct tacka t;
struct tacka* pt;

t.x = 3; /*SAFE*/
t.y = 4; /*SAFE*/

pt = &t;  	
pt->x = 0; /*SAFE*/

pt->y = 5 / t.x; /*SAFE, FLAWED, SAFE*/
}

