; ModuleID = 'while.arow'
source_filename = "while.arow"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @wait(i32) #0 {
  %2 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  %3 = load i32, i32* %2, align 4
  %4 = call i32 @"\01_usleep"(i32 %3)
  ret i32 0
}

declare i32 @"\01_usleep"(i32) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @printnum(i32) #0 {
  %2 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  %3 = load i32, i32* %2, align 4
  %4 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %3)
  ret i32 %4
}

declare i32 @printf(i8*, ...) #1

; Function Attrs: noinline nounwind optnone ssp uwtable
define i32 @usclock() #0 {
  %1 = call i64 @"\01_clock"()
  %2 = uitofp i64 %1 to double
  %3 = fmul double %2, 1.000000e+06
  %4 = fdiv double %3, 1.000000e+06
  %5 = fptosi double %4 to i32
  ret i32 %5
}

declare i64 @"\01_clock"() #1

define i32 @main() {
entry:
  %array = alloca [2 x double]*, i32 2
  %i = alloca i32
  store i32 0, i32* %i
  %var_tmp = load i32, i32* %i
  %ifcond = icmp slt i32 %var_tmp, 5
  br i1 %ifcond, label %loop, label %afterloop

loop:                                             ; preds = %loop, %entry
  %var_tmp1 = load i32, i32* %i
  %add_tmp = add i32 %var_tmp1, 1
  store i32 %add_tmp, i32* %i
  %var_tmp2 = load i32, i32* %i
  %call_tmp = call i32 @printnum(i32 %var_tmp2)
  %var_tmp3 = load i32, i32* %i
  %ifcond4 = icmp slt i32 %var_tmp3, 5
  br i1 %ifcond4, label %loop, label %afterloop

afterloop:                                        ; preds = %loop, %entry
  ret i32 0
}

attributes #0 = { noinline nounwind optnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}
!llvm.module.flags = !{!1, !2}

!0 = !{!"Apple LLVM version 10.0.0 (clang-1000.10.44.4)"}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{i32 7, !"PIC Level", i32 2}
