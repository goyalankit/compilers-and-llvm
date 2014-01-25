All three needs to be done at instruction level.


#### Algebraic identities on : e.g, x + 0 = 0 + x = x and x/x = 1

```c++
int compute (int a, int b)
{
  int result = (a/a);
  unsigned int t = (b/b);
  result *= (b/b);
  result += (b-b);
  result /= result;
  result -= result;
  return result;
}

```

The .ll produced by above code:

```llvm

; Function Attrs: nounwind
define i32 @compute(i32 %a, i32 %b) #0 {
entry:
  %a.addr = alloca i32, align 4
  %b.addr = alloca i32, align 4
  %result = alloca i32, align 4
  %t = alloca i32, align 4
  store i32 %a, i32* %a.addr, align 4
  store i32 %b, i32* %b.addr, align 4
  %0 = load i32* %a.addr, align 4
  %1 = load i32* %a.addr, align 4
  %div = sdiv i32 %0, %1
  store i32 %div, i32* %result, align 4
  %2 = load i32* %b.addr, align 4
  %3 = load i32* %b.addr, align 4
  %div1 = sdiv i32 %2, %3
  store i32 %div1, i32* %t, align 4
  %4 = load i32* %b.addr, align 4
  %5 = load i32* %b.addr, align 4
  %div2 = sdiv i32 %4, %5
  %6 = load i32* %result, align 4
  %mul = mul nsw i32 %6, %div2
  store i32 %mul, i32* %result, align 4
  %7 = load i32* %b.addr, align 4
  %8 = load i32* %b.addr, align 4
  %sub = sub nsw i32 %7, %8
  %9 = load i32* %result, align 4
  %add = add nsw i32 %9, %sub
  store i32 %add, i32* %result, align 4
  %10 = load i32* %result, align 4
  %11 = load i32* %result, align 4
  %div3 = sdiv i32 %11, %10
  store i32 %div3, i32* %result, align 4
  %12 = load i32* %result, align 4
  %13 = load i32* %result, align 4
  %sub4 = sub nsw i32 %13, %12
  store i32 %sub4, i32* %result, align 4
  %14 = load i32* %result, align 4
  ret i32 %14
}


```

Look at the short version of the above code.

```c++
int foo(int a){
  int result = (a/a);
  return 0;
}
```

```llvm
%a.addr = alloca i32, align 4         ;allocating memory for a
%result = alloca i32, align 4         ;allocating memory for result
store i32 %a, i32* %a.addr, align 4   ;initializing memory for a
%0 = load i32* %a.addr, align 4       ;loading a to register 0
%1 = load i32* %a.addr, align 4       ;loading a to register 2
%div = sdiv i32 %0, %1                ;dividing register 0 and register 1. Note the s in sdiv for "signed"
store i32 %div, i32* %result, align 4 ;storing the result in result variable
ret i32 0                             ;returning 0
```

`%div = sdiv i32 %0, %1` needs to be checked for identity. 

`a/a = 1 => a/1 = a` applying the identity yields the same result.

**Similarly for substraction:**

```c++
result += (b - b)
```

```llvm
  %7 = load i32* %b.addr, align 4  ; loading b
  %8 = load i32* %b.addr, align 4  ; loading b again
  %sub = sub nsw i32 %7, %8        ; subtracting b from b
  %9 = load i32* %result, align 4  ; loading result
  %add = add nsw i32 %9, %sub      ; add ing result and result from subtraction
  store i32 %add, i32* %result, align 4 ; storing the above result in result
```

Again look at the instruction level:

`%sub = sub nsw i32 %7, %8` needs to be checked for identity.

`b - b = 0 => b - 0 = b` ;apply the operator and the result should be variable itself.

At the instruction level:



#### Constant folding: e.g, 2 ∗ 4 = 8

#### Strength reductions: e.g, 2 ∗ x = (x + x) or (x << 1)
