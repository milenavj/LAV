#include <stdio.h>
#include <string.h>
#define SIZE 21
int strcspn1(char rec[], char rec1[]) {
  int i, j;
  for (i = 0; rec[i]; i++)
    for (j = 0; rec1[j]; j++)
      if (rec1[j] == rec[i])
        return i;
  return -1;
}
main() {
  char rec[SIZE], rec1[SIZE];
  printf("Unesite prvu rec: ");
  scanf("%s", rec);
  printf("Unesite drugu rec: ");
  scanf("%s", rec1);
  if (strlen(rec) > SIZE || strlen(rec1) > SIZE)
    return 0;
  if (strcspn1(rec, rec1))
    printf("%d\n", strcspn1(rec, rec1));
  else if (strcspn1(rec, rec1) == -1)
    printf("-1\n");
  else
    printf("0\n");

}
