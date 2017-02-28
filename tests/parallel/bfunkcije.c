#include <stdio.h>

int f1(int a) {
    return a+1;
}

int f2(int a) {
    return a+2;
}

int f3(int a) {
    return a+3;
}

int f4(int a) {
    return a+4;
}

int ff1(int a) {
    if(a>0) return f1(a)/a;
    else return a;
}

int ff2(int a) {
    if(a>=0) return f2(a)/a;
    else return -a;
}

int ff3(int a) {
    int b = ff1(a);
    int c = f4(a);
    return b/c;
}

int main() {
    int a,b,c,d,r;
    scanf("%d",&a);
    if(a>=0) { 
        a++;
        b = ff3(a); 
        c = ff2(a);
        d = f2(a);
        r = b/c + a/d;
    }
    else {
        b = ff3(a); 
        c = ff2(a);
        d = f2(a);
        r = b/c + a/d;
    }
    printf("%d\n",r);
    return 0;
}

