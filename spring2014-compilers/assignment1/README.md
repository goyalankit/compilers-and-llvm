### Assignment 1

The goal of this assignment is to familiarize you with the LLVM compiler by implementing a simple LLVM pass that outputs IR instructions followed by the source line number.

Useful Links:
* http://llvm.org/releases/3.4/docs/WritingAnLLVMPass.html
* http://llvm.org/releases/3.4/docs/SourceLevelDebugging.html#c-c-front-end-specific-debug-information
* http://llvm.org/doxygen/AssemblyAnnotationWriter_8h_source.html
* http://llvm.org/doxygen/classllvm_1_1Module.html#aa711d8571cb8212a7693fd4465719c31


Instructions to run the code:

```
make
clang -c -g -emit-llvm loop.c
opt -load ./ModuleInfo.so -module-info loop.bc > /dev/null
```

```
Format Produced:
< INSTRUCTION > ; source code line #: 1
< INSTRUCTION > ; source code line #: 2
< INSTRUCTION > ; source code line #: 6
```
