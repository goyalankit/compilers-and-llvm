#include<stdio.h>

void foo(int b[100], int a[100]){
    int i;
    for(i=99; i > 0; i--){ 
        b[i] = 12;
        if(i-1 > 1)
            a[i] = 21 + b[i-1];
    }
    b[3]  = 23;
    printf("hello %d",b[2]);
}

int main(void){
    int a[100];
    int b[100];
    a[2] = 23;
    printf("%d", a[2]);
    foo(a, b);

    return 0;
}
