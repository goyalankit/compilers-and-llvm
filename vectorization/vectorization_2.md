### Example

```
for(i=0; i < *p; i++){
	A[i] = B[i] + C[i];
	sum = sum + A[i]
}

```

Compiler cannot vectorize above code because it doesn't know:

* if `*p` loop invariant. `*p` may be pointed to by some code that is running in some other thread.
* if B and C are independent or independent of A.
* if there is a dependency in sum. Sum used as Write-after-read and read-after-write.
* may be sum is aliased to A, B / C


**Reduction can be used to explicitly tell compiler**

```
#pragma simd reduction(+:sum)
for(i=0; i < *p; i++){
	A[i] = B[i] + C[i];
	sum = sum + A[i]
}

```

Above pragma directive allows vectorization of above code.

##### reduction

```
reduction(operator:v1, v2, ...)
```
Reduces a vector of values to a single value.
Operator should be assosiative accross iterations.

##### private
```
private(v1, v2, ...)
```
Causes each variable to be private to each iteration of a loop.

##### vectorlength
```
vectorlength(n1, [,n2])
```
Explicitly tell compiler what vector width to use. Must be 2,4,8,or 16.

##### linear
```
linear(v1:step1, v2:step2, ...)
````

Indicate the presence of a induction variable like `count = count + 2` inside the loop. Here the step size is 2 and thus the count is updated by step1 * (vector length) where step1 is 2 in this case.

##### [no]assert
When assert is given it treats vectorization failure as an error and stop compilation. 

Default is no assert where only the warning is printed.


<table><tr><td><a href="vectorization_1.md" >&larr; Previous</a></td><td><a href="vectorization_3.md" >Next &rarr;</a></td></tr></table>