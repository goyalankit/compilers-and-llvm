#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define n 900
double a[n][n], b[n][n], c[n][n];

void compute(int p1, int p2)
{

    int i, j, k;
    int inv;
    for (i = 0;; i++) {
        for (k = 0;; k++) {
            for (j = 0;; j++) {
                inv = p1 * p2 * p1;
                c[i][j] += a[i][k] * b[k][j] + inv;
                if(j>=n-1) break;
            }
            if(k>=n-1) break;
        }
        if(i>=n-1) break;
    }
}

int main(int argc, char *argv[])
{
    int i, j, k;
    clock_t begin, end;
    double time_spent;

   int n_cross_n = n * n;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            a[i][j] = i+j;
            b[i][j] = i-j;
            c[i][j] = 0;
        }
    }

    begin = clock();
    compute(123,1233);
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Time taken: %f\n",time_spent);

    printf("%.1lf\n", c[3][3]);

    return 0;
}
