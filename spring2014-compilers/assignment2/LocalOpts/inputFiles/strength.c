#include<stdio.h>

int compute (int a, int b)
{
  int result = 0;
  
  result += a * 2;
    printf("%d\n", result);
  result += a * 3;
    printf("%d\n", result);
  result += a * 8;
    printf("%d\n", result);
  result -= b / 2;
    printf("%d\n", result);
  result -= b / 4;
    printf("%d\n", result);
  result -= b / 8;
    printf("%d\n", result);
  return result;
}

void main(){
    int result = compute(2,16);
    printf("%d\n", result);
}
