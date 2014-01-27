#include<stdio.h>
int compute (int a, int b)
{
  int result = (a/a);
  unsigned int t = (b/b);
  printf("%d\n",result); //1
  result *= (b/b); //1
  printf("%d\n",result);
  result += (b-b); //1 + 0
  printf("%d\n",result);
  result /= result; //1/1
  printf("%d\n",result);
  result -= result; //1-1 = 0
  printf("%d\n",result);
  result = b + 0; //8
  printf("%d\n",result);
  result = b - 0; //8
  printf("%d\n",result);
  result = 0 - b; //-8
  printf("%d\n",result);
  result = 1 * b; //8
  printf("%d\n",result);
  result = 2 + 2; //4
  printf("%d\n",result);
  return result;
}

void main(){
   int result = compute(4,8);
  printf("%d\n",result);
}
