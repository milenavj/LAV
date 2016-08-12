int result_main(int broj) {
  int cif1, cif2, cif3, cif4;
  if (broj > 999 && broj < 10000) //Ispitujemo dali je dobar unos
      {
    cif1 = broj % 10;           //Dolazimo do poslednje cifre
    cif2 = (broj % 100) / 10;   //Dolazimo do preposlednje cifre
    cif3 = (broj % 1000) / 100; //Dolazimo do druge cifre
    cif4 = broj / 1000;         //Dolazimo do prve cifre
    if (cif4 < cif3 && cif3 < cif2 &&
        cif2 < cif1) //Ispitujemo dali su cifre poredjane po rastucem redu
      return 1;
    else if (cif4 > cif3 && cif3 > cif2 &&
             cif2 >
                 cif1) //ispitujemo dali su cifre poredjane po opadajucem redu
      return 2;
    else
      return 3; //Ispisujemo rezultat da cifre nisu poredjane po nijednom od
                //navedenih redosleda
  } else
    return -1;
}
int assert(int);
int main() {
  int broj;
  scanf("%d", &broj);
  assert(result_main(broj) == check_main(broj));
}
