#include<stdio.h>

int main()
{
	int h,s,d,j,x;
		/*
		h - cifra hiljade
		s - cifra stotine
		d - cifra desetice
		j - cifra jedinice
		x - uneti broj
		*/

		printf("Unesite pozitivan cetvorocifreni broj: \n");
		scanf("%d", &x);

			if (x<1000 || x>9999)
				printf("-1\n");
			else
			{
				h = x/1000;
				s = (x/100)%10;
				d = (x/10)%10;
				j = x%10;

				if (j>d && d>s && s>h)
						printf("Rezultat je: 1\n");
				else if (h>s && s>d && d>j)
						printf("Rezultat je: 2\n");
				else
						printf("Rezultat je: 3\n");

}
return 0;
}
