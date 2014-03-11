#include <stdio.h>
#include <time.h>

int main (int argc, char **argv)
{
    clock_t begin, end;
    double time_spent;
    begin = clock();
    int i, j;
    int result = 0;
    int e,a;
    a = 123;
    for (i = 0; ;i++) {
        for(j = 0; ; ++j) {
            int b =3;
            int c =767676;
            int d = 22525;
            e = (a << 12) * (b << 9) * (c << 10) * (d >> 6);
           
            if(j>200000) break;
        }
        
        result += e;
        if(i>100000) break;
    }
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Time taken: %f\n",time_spent);
    return result;
}
