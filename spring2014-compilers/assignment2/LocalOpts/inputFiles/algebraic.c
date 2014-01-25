int compute (int a, int b)
{
  int result = (a/a);
  unsigned int t = (b/b);
  result *= (b/b);
  result += (b-b);
  result /= result;
  result -= result;
  result = b + 0;
  return result;
}
