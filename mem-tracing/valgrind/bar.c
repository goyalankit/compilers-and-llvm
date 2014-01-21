#include "/work/02681/ankitg/workspace/valgrind/gleipnir/gleipnir.h"
#include<stdio.h>

void foo(int b[10]){
    int i;
    for(i=0; i< 10; i++) b[i] = 12;
    b[3]  = 23;
    printf("hello %d",b[2]);
}

int main(void){
    GL_START_INSTR;
    int a[10];
    a[2] = 23;
    printf("%d", a[2]);
    foo(a);
    GL_STOP_INSTR;
return 0;
}
