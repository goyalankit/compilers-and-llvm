#include <stdio.h>
#include <time.h>

#define SIZE_ARRAY 200

int faint (int x) 
{
    clock_t begin, end;
    double time_spent;
    begin = clock();
   int i;
   int arr[SIZE_ARRAY];
   int err[SIZE_ARRAY][SIZE_ARRAY];
  int a = x + 1;

   for(i = 0; i < SIZE_ARRAY; ++i) {
     int b = a + 2;
     int c = b - 3;
     int d = b + c;
     int e = d * x; 

     arr[i] = e;
     err[i][i] = b;

   }
  return x + a;
}

#define LOOP_COUNT 1000000

int main (int argc, char **argv)
{
    clock_t begin, end;
    double time_spent;
    begin = clock();

  int i, j;
  int result = 0;
  printf ("Multiple Program \n");
  for (i = 0; i < LOOP_COUNT; i++) {
   result += faint (2);
  }
  end = clock();
  time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("Time taken: %f\n",time_spent);
  return result;
}
