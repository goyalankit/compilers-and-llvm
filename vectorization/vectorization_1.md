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


