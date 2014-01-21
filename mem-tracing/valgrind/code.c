#include "/work/02681/ankitg/workspace/valgrind/gleipnir/gleipnir.h"

struct typeA{
  double var1;
  int myArray[10];
};

struct typeA GlStrc;
struct typeA glStrcArray[10];

int glScalar;
int glArray[10];

void func(struct typeA StrcParam[])
{
  int i;

  for(i=0; i<3; i++){
    glStrcArray[i].var1 = glScalar;
    glStrcArray[i].myArray[0] = glArray[0];
    StrcParam[i].var1 = glArray[i];
  }
  return;
}

int main(void)
{
  GL_START_INSTR;

  struct typeA lcStrcArray[5];
  int i, lcScalar, lcArray[10];

  glScalar = 10;
  lcScalar = 20;

  for(i=0; i<2; i++)
    lcArray[i] = glScalar;

  func(lcStrcArray);
  
  GL_STOP_INSTR;
  
  return 0;
}
    
