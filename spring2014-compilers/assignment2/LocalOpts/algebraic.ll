; ModuleID = 'inputFiles/algebraic.c'
target datalayout = "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:32:64-f32:32:32-f64:32:64-v64:64:64-v128:128:128-a0:0:64-f80:32:32-n8:16:32-S128"
target triple = "i386-pc-linux-gnu"

; Function Attrs: nounwind
define i32 @compute(i32 %a, i32 %b) #0 {
entry:
  %a.addr = alloca i32, align 4
  %b.addr = alloca i32, align 4
  %result = alloca i32, align 4
  %f = alloca float, align 4
  %resultf = alloca float, align 4
  %t = alloca i32, align 4
  store i32 %a, i32* %a.addr, align 4
  store i32 %b, i32* %b.addr, align 4
  %0 = load i32* %a.addr, align 4
  %1 = load i32* %a.addr, align 4
  %div = sdiv i32 %0, %1
  store i32 %div, i32* %result, align 4
  store float 1.000000e+00, float* %f, align 4
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
  %14 = load i32* %b.addr, align 4
  %add5 = add nsw i32 %14, 0
  store i32 %add5, i32* %result, align 4
  %15 = load i32* %b.addr, align 4
  %sub6 = sub nsw i32 %15, 0
  store i32 %sub6, i32* %result, align 4
  %16 = load float* %f, align 4
  %add7 = fadd float %16, 0.000000e+00
  store float %add7, float* %resultf, align 4
  store i32 4, i32* %result, align 4
  %17 = load i32* %result, align 4
  ret i32 %17
}

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"clang version 3.4 (tags/RELEASE_34/final)"}
