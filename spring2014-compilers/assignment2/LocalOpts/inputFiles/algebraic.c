int compute (int a, int b)
{
  int result = (a/a); //algebraic identity result = 1

  result *= (b/b); //algebraic identity result = 1 * 1
  result += (b-b); //algebraic identity result = 1 + 0
  result /= result; //algebraic identity result = 1 / 1
  result -= result; //algebraic identity result = 1 - 1
  result += 23;
  return result;
}
