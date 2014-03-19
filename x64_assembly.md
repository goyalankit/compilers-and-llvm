Notes on Introduction to x64 Assembly
---


* 16 general purpose registers.

  
  ```assembly
  Names:
  RAX, RBX, RCX, RDX, RBP, RSI, RDI, and RSP -> historical reasons
  R8-R15
  ```
  
* Instruction pointer: RIP points to the next instruction to be executed
* RSP points to the last item pushed onto the stack, which grows toward lower addresses.
