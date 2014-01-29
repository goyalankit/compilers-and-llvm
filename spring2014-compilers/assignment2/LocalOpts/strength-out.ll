; ModuleID = 'strength-out.bc'
target datalayout = "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:32:64-f32:32:32-f64:32:64-v64:64:64-v128:128:128-a0:0:64-f80:32:32-n8:16:32-S128"
target triple = "i386-pc-linux-gnu"

@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1

; Function Attrs: nounwind
define i32 @compute(i32 %a, i32 %b) #0 {
entry:
  %0 = shl i32 %a, 1
  %mul1 = mul nsw i32 %a, 3
  %add2 = add nsw i32 %0, %mul1
  %1 = shl i32 %a, 3
  %add4 = add nsw i32 %add2, %1
  %2 = ashr i32 %b, 1
  %sub = sub nsw i32 %add4, %2
  %3 = ashr i32 %b, 2
  %sub6 = sub nsw i32 %sub, %3
  %4 = ashr i32 %b, 3
  %sub8 = sub nsw i32 %sub6, %4
  ret i32 %sub8
}

; Function Attrs: nounwind
define void @main() #0 {
entry:
  %call = call i32 @compute(i32 2, i32 20)
  %call1 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([3 x i8]* @.str, i32 0, i32 0), i32 %call)
  ret void
}

declare i32 @printf(i8*, ...) #1

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"clang version 3.4 (tags/RELEASE_34/final)"}
