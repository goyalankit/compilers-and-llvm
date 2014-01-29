int compute ()
{
  int result = 0;
  int a = 2;
  int b = 3;
  int c = 4 + a + b; //c=9
  
  result += a; //res = 0 + 2
  result += b; //2 + 3
  result *= c; //5*9
  result /= 2; //45/2=22
  return result;
}
