### Vectorization Intel Webinar notes

**Problem Statement:** Vector widths are increasing and extenstions to languages are needed to give best performance on new architectures.

Different vendors are creating processors with multiple cores and within the cores vector width is increasing. Registers can bring in data in larger quantities and perform simultaneous operations on these quantitities.

**Solutions:** Multiple methods are avaialable to developers to program explicit vector programming.

Provide language extensions to simplify vector parallelism. Enable developers to extract more performane from SIMD processors.

---
```
		 --------------------
		| Auto-vectorization |
		 --------------------			
 
 				 |
 				 |
 
    	------------------------------
	    | Explicit vector programming |
    	------------------------------

```
**Three ways to do explicit vectorization**

* Array Notation
* Elemental Function
* SIMD pragma

Auto-vectorization is limited due to serial nature of the code.

2-4x speedup by applying explicit vectorization on Xeon-phi

<table><tr><td>&larr; Previous</td><td><a href="vectorization_1.md" >Next &rarr;</a></td></tr></table>
