Data flow:
Flow of data values from one statement to another.

**Control flow:**
Sequencing of operations. Evaluation of then or else code depends on the if condition.

**Control flow analysis:**
- Discover the control flow within a __procedure__
- Build a representation of control flow(loops,etc).

***
High level: Control flow is implicit in AST
Low level: Use of a **control flow graph**(CFG).
		-- Nodes represent statements
		-- Edges represent explicit flow of control.

Source code
1  a := 0				 ;start of block
2  b := a * b    ;end of block
3  L1: c := b/d  ;Entry point, start of block
4  if c < x goto ;Exit point, end of block
5  e := b / c		 ;start of block
6  f := e + 1 	 ;end of block
7  L2: g := f    ;Entry point; start of block
8  h := t - g    
9  if e > 0 goto L3  ;Exit Point, end of block
10 goto L1           ;Exit point, end of block
11 L3: return        ;Entry point, start and end of block**Basic Blocks**: A sequence of straight line code that can be entered only at the beginning and exited only at the end.

**Extended basic blocks**: A maximal sequence of instructions that has no merge points in it.(except perhaps in the leader)
		- Single entry multiple exits.
		- Increases the scope of any local analysis or transformation that "flow forwards"

**Reverse Extended basic block**
	



Review:
**Overcoming the challenges to Feedback-Directed Optimization**
Paper presents quite an interesting view on feedback based optimizations.

Branch-based phase behavior is very similar to the problem of predicting best rate for transmission in networks. The main problem is what amount of interval you need to consider to predict the best rate. In one of the papers, authors show that the mutual information decreases between subsequent dropped packets over a long interval. This seems to be the behavior of phased behavior. If one considers a short enough interval to predict the branch, I think a good performance can be achieved. 

I am not convinced mutable executables are a very good idea due to very reason that a correct code could turn buggy and I am not sure how would one go about debugging an executable that was changed based on the usage pattern.

