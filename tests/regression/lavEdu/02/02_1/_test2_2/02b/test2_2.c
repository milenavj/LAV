#include <stdio.h>
#define MAXD 100

int indeks_maksimuma(float a[], int n){
	int max = 0; //max = n-1 (alternativni metod - idi od kraja niza)
	int i;
	for(i=1; i<n; i++) //for(i=n-2; i>=0; i--) (ako bi isao od kraja niza)
		if(a[i] >= a[max]) //i kad je jednak trenutnom maksimumu
			max = i; //treba da postane novi maksimum
	return max; //jer se gleda POSLEDNJE pojavljivanje, a petlja ide od pocetka niza
	//(ako bi isao od kraja niza onda gore u if-u ne bi bio operator >= nego >)
}

int indeks_minimuma(float a[], int n){
	int min = 0;
	int i;
	for(i=1; i<n; i++)
		if(a[i] <= a[min]) //i ovde vazi isto pravilo kao za indeks_maksimuma
			min = i; //jer potreban je indeks poslednjeg pojavljivanja!
	return min;
}

int main(){
	int n, i, min, max;
	float niz[MAXD];
	
	printf("Uneti dimenziju niza: ");
	scanf("%d", &n);
	if(n<=0 || n>MAXD){ //provera ispravnosti dimenzije
		printf("-1\n");
		return 0; //izlaz odmah, jer je neispravan
	}
	
	printf("Uneti niz: ");
	for(i=0; i<n; i++)
		scanf("%f", &niz[i]); //unosenje jednog po jednog elementa - moglo je pisati i niz+i umesto &niz[i]
	
	//poziv funkcija za indeks minimuma i maksimuma
	min = indeks_minimuma(niz, n);
	max = indeks_maksimuma(niz, n);
	
	printf("%d\n%d\n", min, max); //ispisuje prvo index minimuma, pa maksimuma
	if(min > max) //provera da li je indeks minimuma strogo veci od ovog drugog
		printf("1\n");
	else
		printf("0\n");
	
	return 0;
}

