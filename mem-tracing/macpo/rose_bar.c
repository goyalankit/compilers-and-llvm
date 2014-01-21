void indigo__create_map();
#include<stdio.h>
#include "mrt.h" 

void foo(int b[100UL],int a[100UL])
{
  int i;
  for (i = 99; i > 0; i--) {
    indigo__gen_trace_c(2,6,b,((void *)(&b[i])),0);
    b[i] = 12;
    if ((i - 1) > 1) {
      indigo__gen_trace_c(2,8,a,((void *)(&a[i])),1);
      indigo__gen_trace_c(1,8,b,((void *)(&b[i - 1])),0);
      a[i] = (21 + b[i - 1]);
    }
  }
  indigo__gen_trace_c(2,10,b,((void *)(&b[3])),0);
  b[3] = 23;
  printf("hello %d",b[2]);
}

int main()
{
  int a[100UL];
  int b[100UL];
  indigo__init_(1);
  indigo__create_map();
  a[2] = 23;
  printf("%d",a[2]);
  foo(a,b);
  return 0;
}

void indigo__create_map()
{
  indigo__write_idx_c("b",1);
  indigo__write_idx_c("a",1);
}
