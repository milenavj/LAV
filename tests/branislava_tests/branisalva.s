; ModuleID = 'branisalva.c'
target datalayout = "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:32:64-f32:32:32-f64:32:64-v64:64:64-v128:128:128-a0:0:64-f80:32:32-n8:16:32-S128"
target triple = "i386-pc-linux-gnu"

; Function Attrs: nounwind
define i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %c = alloca i32, align 4
  store i32 0, i32* %retval
  call void @llvm.dbg.declare(metadata !{i32* %a}, metadata !9), !dbg !10
  call void @llvm.dbg.declare(metadata !{i32* %b}, metadata !11), !dbg !10
  call void @llvm.dbg.declare(metadata !{i32* %c}, metadata !12), !dbg !10
  %0 = load i32* %a, align 4, !dbg !13
  %shl = shl i32 %0, 3, !dbg !13
  %1 = load i32* %b, align 4, !dbg !13
  %div = sdiv i32 %shl, %1, !dbg !13
  store i32 %div, i32* %a, align 4, !dbg !13
  %2 = load i32* %c, align 4, !dbg !14
  %3 = load i32* %c, align 4, !dbg !14
  %shl1 = shl i32 %2, %3, !dbg !14
  %4 = load i32* %a, align 4, !dbg !14
  %div2 = sdiv i32 %shl1, %4, !dbg !14
  store i32 %div2, i32* %b, align 4, !dbg !14
  %5 = load i32* %b, align 4, !dbg !15
  %6 = load i32* %a, align 4, !dbg !15
  %shl3 = shl i32 %5, %6, !dbg !15
  %7 = load i32* %b, align 4, !dbg !15
  %div4 = sdiv i32 %shl3, %7, !dbg !15
  store i32 %div4, i32* %c, align 4, !dbg !15
  %8 = load i32* %a, align 4, !dbg !16
  %shl5 = shl i32 %8, 3, !dbg !16
  %9 = load i32* %b, align 4, !dbg !16
  %div6 = sdiv i32 %shl5, %9, !dbg !16
  store i32 %div6, i32* %a, align 4, !dbg !16
  %10 = load i32* %c, align 4, !dbg !17
  %11 = load i32* %c, align 4, !dbg !17
  %shl7 = shl i32 %10, %11, !dbg !17
  %12 = load i32* %a, align 4, !dbg !17
  %div8 = sdiv i32 %shl7, %12, !dbg !17
  store i32 %div8, i32* %b, align 4, !dbg !17
  %13 = load i32* %b, align 4, !dbg !18
  %14 = load i32* %a, align 4, !dbg !18
  %shl9 = shl i32 %13, %14, !dbg !18
  %15 = load i32* %b, align 4, !dbg !18
  %div10 = sdiv i32 %shl9, %15, !dbg !18
  store i32 %div10, i32* %c, align 4, !dbg !18
  %16 = load i32* %a, align 4, !dbg !19
  %shl11 = shl i32 %16, 3, !dbg !19
  %17 = load i32* %b, align 4, !dbg !19
  %div12 = sdiv i32 %shl11, %17, !dbg !19
  store i32 %div12, i32* %a, align 4, !dbg !19
  %18 = load i32* %c, align 4, !dbg !20
  %19 = load i32* %c, align 4, !dbg !20
  %shl13 = shl i32 %18, %19, !dbg !20
  %20 = load i32* %a, align 4, !dbg !20
  %div14 = sdiv i32 %shl13, %20, !dbg !20
  store i32 %div14, i32* %b, align 4, !dbg !20
  %21 = load i32* %b, align 4, !dbg !21
  %22 = load i32* %a, align 4, !dbg !21
  %shl15 = shl i32 %21, %22, !dbg !21
  %23 = load i32* %b, align 4, !dbg !21
  %div16 = sdiv i32 %shl15, %23, !dbg !21
  store i32 %div16, i32* %c, align 4, !dbg !21
  ret i32 0, !dbg !22
}

; Function Attrs: nounwind readnone
declare void @llvm.dbg.declare(metadata, metadata) #1

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf"="true" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone }

!llvm.dbg.cu = !{!0}

!0 = metadata !{i32 786449, metadata !1, i32 12, metadata !"clang version 3.3 (tags/RELEASE_33/final)", i1 false, metadata !"", i32 0, metadata !2, metadata !2, metadata !3, metadata !2, metadata !2, metadata !""} ; [ DW_TAG_compile_unit ] [/home/milena/lav/LAV/tests/branislava_tests/branisalva.c] [DW_LANG_C99]
!1 = metadata !{metadata !"branisalva.c", metadata !"/home/milena/lav/LAV/tests/branislava_tests"}
!2 = metadata !{i32 0}
!3 = metadata !{metadata !4}
!4 = metadata !{i32 786478, metadata !1, metadata !5, metadata !"main", metadata !"main", metadata !"", i32 3, metadata !6, i1 false, i1 true, i32 0, i32 0, null, i32 0, i1 false, i32 ()* @main, null, null, metadata !2, i32 3} ; [ DW_TAG_subprogram ] [line 3] [def] [main]
!5 = metadata !{i32 786473, metadata !1}          ; [ DW_TAG_file_type ] [/home/milena/lav/LAV/tests/branislava_tests/branisalva.c]
!6 = metadata !{i32 786453, i32 0, i32 0, metadata !"", i32 0, i64 0, i64 0, i64 0, i32 0, null, metadata !7, i32 0, i32 0} ; [ DW_TAG_subroutine_type ] [line 0, size 0, align 0, offset 0] [from ]
!7 = metadata !{metadata !8}
!8 = metadata !{i32 786468, null, null, metadata !"int", i32 0, i64 32, i64 32, i64 0, i32 0, i32 5} ; [ DW_TAG_base_type ] [int] [line 0, size 32, align 32, offset 0, enc DW_ATE_signed]
!9 = metadata !{i32 786688, metadata !4, metadata !"a", metadata !5, i32 5, metadata !8, i32 0, i32 0} ; [ DW_TAG_auto_variable ] [a] [line 5]
!10 = metadata !{i32 5, i32 0, metadata !4, null}
!11 = metadata !{i32 786688, metadata !4, metadata !"b", metadata !5, i32 5, metadata !8, i32 0, i32 0} ; [ DW_TAG_auto_variable ] [b] [line 5]
!12 = metadata !{i32 786688, metadata !4, metadata !"c", metadata !5, i32 5, metadata !8, i32 0, i32 0} ; [ DW_TAG_auto_variable ] [c] [line 5]
!13 = metadata !{i32 7, i32 0, metadata !4, null}
!14 = metadata !{i32 8, i32 0, metadata !4, null}
!15 = metadata !{i32 9, i32 0, metadata !4, null}
!16 = metadata !{i32 10, i32 0, metadata !4, null}
!17 = metadata !{i32 11, i32 0, metadata !4, null}
!18 = metadata !{i32 12, i32 0, metadata !4, null}
!19 = metadata !{i32 13, i32 0, metadata !4, null}
!20 = metadata !{i32 14, i32 0, metadata !4, null}
!21 = metadata !{i32 15, i32 0, metadata !4, null}
!22 = metadata !{i32 18, i32 0, metadata !4, null}
