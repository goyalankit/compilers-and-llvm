; ModuleID = 'algebraic.bc'
target datalayout = "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:32:64-f32:32:32-f64:32:64-v64:64:64-v128:128:128-a0:0:64-f80:32:32-n8:16:32-S128"
target triple = "i386-pc-linux-gnu"

; Function Attrs: nounwind
define i32 @compute(i32 %a, i32 %b) #0 {
entry:
  %a.addr = alloca i32, align 4
  %b.addr = alloca i32, align 4
  %result = alloca i32, align 4
  store i32 %a, i32* %a.addr, align 4
  call void @llvm.dbg.declare(metadata !{i32* %a.addr}, metadata !12), !dbg !13
  store i32 %b, i32* %b.addr, align 4
  call void @llvm.dbg.declare(metadata !{i32* %b.addr}, metadata !14), !dbg !13
  call void @llvm.dbg.declare(metadata !{i32* %result}, metadata !15), !dbg !16
  %0 = load i32* %a.addr, align 4, !dbg !16
  %1 = load i32* %a.addr, align 4, !dbg !16
  %div = sdiv i32 %0, %1, !dbg !16
  store i32 %div, i32* %result, align 4, !dbg !16
  %2 = load i32* %b.addr, align 4, !dbg !17
  %3 = load i32* %b.addr, align 4, !dbg !17
  %div1 = sdiv i32 %2, %3, !dbg !17
  %4 = load i32* %result, align 4, !dbg !17
  %mul = mul nsw i32 %4, %div1, !dbg !17
  store i32 %mul, i32* %result, align 4, !dbg !17
  %5 = load i32* %b.addr, align 4, !dbg !18
  %6 = load i32* %b.addr, align 4, !dbg !18
  %sub = sub nsw i32 %5, %6, !dbg !18
  %7 = load i32* %result, align 4, !dbg !18
  %add = add nsw i32 %7, %sub, !dbg !18
  store i32 %add, i32* %result, align 4, !dbg !18
  %8 = load i32* %result, align 4, !dbg !19
  %9 = load i32* %result, align 4, !dbg !19
  %div2 = sdiv i32 %9, %8, !dbg !19
  store i32 %div2, i32* %result, align 4, !dbg !19
  %10 = load i32* %result, align 4, !dbg !20
  %11 = load i32* %result, align 4, !dbg !20
  %sub3 = sub nsw i32 %11, %10, !dbg !20
  store i32 %sub3, i32* %result, align 4, !dbg !20
  %12 = load i32* %result, align 4, !dbg !21
  ret i32 %12, !dbg !21
}

; Function Attrs: nounwind readnone
declare void @llvm.dbg.declare(metadata, metadata) #1

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!9, !10}
!llvm.ident = !{!11}

!0 = metadata !{i32 786449, metadata !1, i32 12, metadata !"clang version 3.4 (tags/RELEASE_34/final)", i1 false, metadata !"", i32 0, metadata !2, metadata !2, metadata !3, metadata !2, metadata !2, metadata !""} ; [ DW_TAG_compile_unit ] [/home/user/compiler-notes-and-code/spring2014-compilers/assignment2/LocalOpts/inputFiles/algebraic.c] [DW_LANG_C99]
!1 = metadata !{metadata !"inputFiles/algebraic.c", metadata !"/home/user/compiler-notes-and-code/spring2014-compilers/assignment2/LocalOpts"}
!2 = metadata !{i32 0}
!3 = metadata !{metadata !4}
!4 = metadata !{i32 786478, metadata !1, metadata !5, metadata !"compute", metadata !"compute", metadata !"", i32 1, metadata !6, i1 false, i1 true, i32 0, i32 0, null, i32 256, i1 false, i32 (i32, i32)* @compute, null, null, metadata !2, i32 2} ; [ DW_TAG_subprogram ] [line 1] [def] [scope 2] [compute]
!5 = metadata !{i32 786473, metadata !1}          ; [ DW_TAG_file_type ] [/home/user/compiler-notes-and-code/spring2014-compilers/assignment2/LocalOpts/inputFiles/algebraic.c]
!6 = metadata !{i32 786453, i32 0, null, metadata !"", i32 0, i64 0, i64 0, i64 0, i32 0, null, metadata !7, i32 0, null, null, null} ; [ DW_TAG_subroutine_type ] [line 0, size 0, align 0, offset 0] [from ]
!7 = metadata !{metadata !8, metadata !8, metadata !8}
!8 = metadata !{i32 786468, null, null, metadata !"int", i32 0, i64 32, i64 32, i64 0, i32 0, i32 5} ; [ DW_TAG_base_type ] [int] [line 0, size 32, align 32, offset 0, enc DW_ATE_signed]
!9 = metadata !{i32 2, metadata !"Dwarf Version", i32 4}
!10 = metadata !{i32 1, metadata !"Debug Info Version", i32 1}
!11 = metadata !{metadata !"clang version 3.4 (tags/RELEASE_34/final)"}
!12 = metadata !{i32 786689, metadata !4, metadata !"a", metadata !5, i32 16777217, metadata !8, i32 0, i32 0} ; [ DW_TAG_arg_variable ] [a] [line 1]
!13 = metadata !{i32 1, i32 0, metadata !4, null}
!14 = metadata !{i32 786689, metadata !4, metadata !"b", metadata !5, i32 33554433, metadata !8, i32 0, i32 0} ; [ DW_TAG_arg_variable ] [b] [line 1]
!15 = metadata !{i32 786688, metadata !4, metadata !"result", metadata !5, i32 3, metadata !8, i32 0, i32 0} ; [ DW_TAG_auto_variable ] [result] [line 3]
!16 = metadata !{i32 3, i32 0, metadata !4, null}
!17 = metadata !{i32 5, i32 0, metadata !4, null}
!18 = metadata !{i32 6, i32 0, metadata !4, null}
!19 = metadata !{i32 7, i32 0, metadata !4, null}
!20 = metadata !{i32 8, i32 0, metadata !4, null} ; [ DW_TAG_imported_declaration ]
!21 = metadata !{i32 9, i32 0, metadata !4, null}
