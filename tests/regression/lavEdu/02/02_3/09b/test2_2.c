#include <stdio.h>
#include <stdlib.h>
int fmin(float niz[], int n){
  int i, last = 0; 
  float min = niz[0];
  for(i = 0; i < n; i++){
    if(niz[i] < min){
      min = niz[i];
      last = i;
    }
    else if(min == niz[i]){
      last = i;
    }
  }
  return last;
}
int fmax(float niz[], int n){
  int i, last = 0; 
  float max = niz[0];
  for(i = 0; i < n; i++){
    if(niz[i] > max){
      max = niz[i];
      last = i;
    }
    else if(max == niz[i]){
      last = i;
    }
  }
  return last;
}
int main(){
  float niz[100];
  int n, i; 
  printf("Uneti dimenziju niza: ");
  scanf("%d", &n);
  if(n <= 0){
    printf("-1\n");
    exit(1);
  }
  for(i = 0; i < n; i++){
    scanf("%f", &niz[i]);
  }
  int min = fmin(niz, n);
  int max = fmax(niz, n);
  printf("%d\n%d\n", min ,max);
  if(min > max){
    printf("1\n");
  }
  else{
    printf("0\n");
  }
  return 0;
}
