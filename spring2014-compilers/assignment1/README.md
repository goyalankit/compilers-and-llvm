### Assignment 1


Instructions to run the code:

1. make
2. clang -c -g -emit-llvm loop.c
3. opt -load ./ModuleInfo.so -module-info loop.bc > /dev/null

Format Produced:
< INSTRUCTION > ; source code line #: 1
< INSTRUCTION > ; source code line #: 2
< INSTRUCTION > ; source code line #: 6
