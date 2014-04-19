### Compilers and LLVM

---

* **basic-compiler-notes** contains course notes from first few lectures from coursera's course on compilers
* **vectorization** contains notes on vectorization.
* **mem-tracing** this has been moved to its own repository. ignore this dir.

* **spring-2014-compilers/assignment1:** 
  * **Module pass** to print source code line number as comments in LLVM IR using  `AssemblyAnnotationWriter` class.

* **spring-2014-compilers/assignment2:** 
  * **Function Info pass**: that prints function name, argument size, number of call sites, number of basic blocks, number of instructions for a given function.
  * **LocalOpts Pass**: is an transform optimization pass (changes IR). It applies optimizations such as constant folding, strength reduction and algebraic identity.

* **spring-2014-compilers/assignment3:** 
  * **dataflow.h:** implements a dataflow framework with virtual methods that can be implemented to forward (reaching definitions) and backward (liveness) analysis.
  * **Live**: pass uses dataflow framework to print live variables above each instruction in IR (SSA form)
  * **Reach**: pass uses dataflow framework to print reaching variables above each instruction in IR (SSA form)

* **spring-2014-compilers/assignment4:** 
  * **DCE**: pass performs dead code elimination. It uses dataflow frameword developed in the previous assignment.
  * **licm**: pass that performs loop invariant code motion. It also uses above dataflow framework.



#### Lessons:

It would be better to implement dataflow framework using function pointers for `meet` and `transfer` functions, otherwise you can only use them once if implemented using virtual methods.
