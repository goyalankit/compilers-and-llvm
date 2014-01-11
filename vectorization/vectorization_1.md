### What is SIMD and vectorization

**S**ingle **I**nstruction **M**ultiple **D**ata (SIMD)

```

| |			| | | |
| |			| | | |
---	  		-------
| |			| | | |

scalar		vector

```

Explicit vector programming allows you to write more portable code. 

**Example:**

```c++

for(int i=0; i<N; i++){
  A[i] = B[i] + C[i]  
}

```

Above code doesn't guarantee vectorization. Most compilers will vectorize it though.

#### Explicit Vectorization

**SIMD Pragma/Directive**

Explicitly telling the compiler to vectorize. Knowing that B is completly independent of C and C is independent of A. Articulating this to compiler.


```c++
#pragma simd
for(int i=0; i<N; i++){
  A[i] = B[i] + C[i]  
}

```

**Array Notation**

Articulating the same thing.

```c++
A[:] = B[:] + C[:];

```

**Elemental Function**

```c++
__declspec(vector) //This turn foo into a vectorized function

float foo(float B, float C){
	return B + C;
}

...

for(int i=0; i<N; i++){
  A[i] = foo(B[i], C[i]);  
}

```