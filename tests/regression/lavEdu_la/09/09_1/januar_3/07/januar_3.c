#include<stdio.h>
#include<stdlib.h>

typedef struct razlomak RAZLOMAK;

struct razlomak{
	int imenilac;   //struktura razlomka
	int brojilac;
	};

unsigned manji_ili_jednak(RAZLOMAK a,RAZLOMAK b){
	if (((double)a.imenilac/a.brojilac)<=((double)b.imenilac/b.brojilac)) return 1; //proverava koji
	else return 0;                                                                  //je veci
}

void ucitaj_razlomak(RAZLOMAK razlomci[],unsigned n){
	int i=0;
	for(i=0;i<n;i++){ //ucitava sve razlomke
			scanf("%d",&razlomci[i].imenilac);
			scanf("%d",&razlomci[i].brojilac);
			}
}

void ispisi_razlomak(RAZLOMAK razlomci[],unsigned n){
	int i=0;
	for(i=0;i<n;i++){    //ispisuje razlomku
			printf("%d/%d ",razlomci[i].imenilac,razlomci[i].brojilac);
			}
	printf("\n");   // na kraju prelazi u novi red
}

RAZLOMAK maksimum(RAZLOMAK * razlomci,unsigned n){
	int i;
	RAZLOMAK r;
	r.imenilac=razlomci[0].imenilac; //nulti u nizu namesta na maksimum
	r.brojilac=razlomci[0].brojilac;
	for(i=1;i<n;i++)                        // pretrezuje maksimum
		if (manji_ili_jednak(r,razlomci[i])){
							r.imenilac=razlomci[i].imenilac;
							r.brojilac=razlomci[i].brojilac;
							}
	return r;
}


int main(){
	int i,n;
	RAZLOMAK a,b,r;
	RAZLOMAK *c;
	scanf("%d%d%d%d",&a.imenilac,&a.brojilac,&b.imenilac,&b.brojilac);
	i=manji_ili_jednak(a,b);
	printf("(b): %d\n",i);
	scanf("%d",&n);
	c=(RAZLOMAK *)malloc(n*2*sizeof(int));   //dinamicki alocira niz
	if (c==NULL) { printf("GRESKA!\n"); return 1; } // ispisuje ako ima greska
	ucitaj_razlomak(c,n);
	printf("(d): ");
	ispisi_razlomak(c,n);
	r=maksimum(c,n);
	printf("(e): %d/%d\n",r.imenilac,r.brojilac);
	free(c);
	return 0;
}


