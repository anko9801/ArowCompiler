; ModuleID = 'test9.arow'
source_filename = "test9.arow"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

; Function Attrs: nounwind ssp uwtable
define i32 @wait(i32) local_unnamed_addr #0 {
  %2 = tail call i32 @"\01_usleep"(i32 %0) #3
  ret i32 0
}

declare i32 @"\01_usleep"(i32) local_unnamed_addr #1

; Function Attrs: nounwind ssp uwtable
define i32 @printnum(i32) local_unnamed_addr #0 {
  %2 = tail call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i64 0, i64 0), i32 %0)
  ret i32 %2
}

; Function Attrs: nounwind
declare i32 @printf(i8* nocapture readonly, ...) local_unnamed_addr #2

; Function Attrs: nounwind ssp uwtable
define i32 @usclock() local_unnamed_addr #0 {
  %1 = tail call i64 @"\01_clock"() #3
  %2 = uitofp i64 %1 to double
  %3 = fmul double %2, 1.000000e+06
  %4 = fdiv double %3, 1.000000e+06
  %5 = fptosi double %4 to i32
  ret i32 %5
}

declare i64 @"\01_clock"() local_unnamed_addr #1

define i32 @main() {
entry:
  %array = alloca [2 x double]*, i32 2
  %cnt = alloca i32
  store i32 13, i32* %cnt
  %var_tmp = load i32, i32* %cnt
  %ifcond = icmp slt i32 %var_tmp, 20
  br label %loop

loop:                                             ; preds = %loop, %entry
  %var_tmp1 = load i32, i32* %cnt
  %add_tmp = add i32 %var_tmp1, 1
  store i32 %add_tmp, i32* %cnt
  br i1 %ifcond, label %loop, label %afterloop

afterloop:                                        ; preds = %loop
  %var_tmp2 = load i32, i32* %cnt
  %call_tmp = call i32 @printnum(i32 %var_tmp2)
  %var_tmp3 = load i32, i32* %cnt
  ret i32 %var_tmp3
}

attributes #0 = { nounwind ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind }

!llvm.ident = !{!0}
!llvm.module.flags = !{!1, !2}

!0 = !{!"Apple LLVM version 10.0.0 (clang-1000.10.44.4)"}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{i32 7, !"PIC Level", i32 2}
