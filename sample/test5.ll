; ModuleID = 'test5.arow'
source_filename = "test5.arow"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

%struct.rpi_gpio = type { i64, i32, i8*, i32* }

@gpio = local_unnamed_addr global %struct.rpi_gpio { i64 1059061760, i32 0, i8* null, i32* null }, align 8
@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@.str.1 = private unnamed_addr constant [9 x i8] c"/dev/mem\00", align 1
@.str.2 = private unnamed_addr constant [48 x i8] c"Failed to open /dev/mem, try change permission.\00", align 1
@.str.3 = private unnamed_addr constant [5 x i8] c"mmap\00", align 1
@str = private unnamed_addr constant [21 x i8] c"Failed to blink LED.\00"

; Function Attrs: nounwind ssp uwtable
define i32 @msleep(i32) local_unnamed_addr #0 {
  %2 = tail call i32 @"\01_usleep"(i32 10) #3
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

; Function Attrs: nounwind ssp uwtable
define i32 @MapGPIO() local_unnamed_addr #0 {
  %1 = tail call i32 (i8*, i32, ...) @"\01_open"(i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str.1, i64 0, i64 0), i32 130) #3
  store i32 %1, i32* getelementptr inbounds (%struct.rpi_gpio, %struct.rpi_gpio* @gpio, i64 0, i32 1), align 8, !tbaa !3
  %2 = icmp slt i32 %1, 0
  br i1 %2, label %3, label %4

; <label>:3:                                      ; preds = %0
  tail call void @perror(i8* getelementptr inbounds ([48 x i8], [48 x i8]* @.str.2, i64 0, i64 0)) #4
  br label %10

; <label>:4:                                      ; preds = %0
  %5 = load i64, i64* getelementptr inbounds (%struct.rpi_gpio, %struct.rpi_gpio* @gpio, i64 0, i32 0), align 8, !tbaa !10
  %6 = tail call i8* @"\01_mmap"(i8* null, i64 4096, i32 3, i32 1, i32 %1, i64 %5) #3
  store i8* %6, i8** getelementptr inbounds (%struct.rpi_gpio, %struct.rpi_gpio* @gpio, i64 0, i32 2), align 8, !tbaa !11
  %7 = icmp eq i8* %6, inttoptr (i64 -1 to i8*)
  br i1 %7, label %8, label %9

; <label>:8:                                      ; preds = %4
  tail call void @perror(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.3, i64 0, i64 0)) #4
  br label %10

; <label>:9:                                      ; preds = %4
  store i8* %6, i8** bitcast (i32** getelementptr inbounds (%struct.rpi_gpio, %struct.rpi_gpio* @gpio, i64 0, i32 3) to i8**), align 8, !tbaa !12
  br label %10

; <label>:10:                                     ; preds = %9, %8, %3
  %11 = phi i32 [ 1, %3 ], [ 1, %8 ], [ 0, %9 ]
  ret i32 %11
}

declare i32 @"\01_open"(i8*, i32, ...) local_unnamed_addr #1

; Function Attrs: nounwind
declare void @perror(i8* nocapture readonly) local_unnamed_addr #2

declare i8* @"\01_mmap"(i8*, i64, i32, i32, i32, i64) local_unnamed_addr #1

; Function Attrs: nounwind ssp uwtable
define void @GPIOclear() local_unnamed_addr #0 {
  %1 = load i8*, i8** getelementptr inbounds (%struct.rpi_gpio, %struct.rpi_gpio* @gpio, i64 0, i32 2), align 8, !tbaa !11
  %2 = tail call i32 @"\01_munmap"(i8* %1, i64 4096) #3
  %3 = load i32, i32* getelementptr inbounds (%struct.rpi_gpio, %struct.rpi_gpio* @gpio, i64 0, i32 1), align 8, !tbaa !3
  %4 = tail call i32 @"\01_close"(i32 %3) #3
  ret void
}

declare i32 @"\01_munmap"(i8*, i64) local_unnamed_addr #1

declare i32 @"\01_close"(i32) local_unnamed_addr #1

; Function Attrs: nounwind ssp uwtable
define void @BlinkLED(i32) local_unnamed_addr #0 {
  %2 = srem i32 %0, 10
  %3 = mul nsw i32 %2, 3
  %4 = shl i32 1, %3
  %5 = load i32*, i32** getelementptr inbounds (%struct.rpi_gpio, %struct.rpi_gpio* @gpio, i64 0, i32 3), align 8, !tbaa !12
  %6 = sdiv i32 %0, 10
  %7 = sext i32 %6 to i64
  %8 = getelementptr inbounds i32, i32* %5, i64 %7
  store volatile i32 %4, i32* %8, align 4, !tbaa !13
  %9 = shl i32 1, %0
  %10 = getelementptr inbounds i32, i32* %5, i64 7
  store volatile i32 %9, i32* %10, align 4, !tbaa !13
  %11 = tail call i32 @"\01_usleep"(i32 500000) #3
  %12 = load i32*, i32** getelementptr inbounds (%struct.rpi_gpio, %struct.rpi_gpio* @gpio, i64 0, i32 3), align 8, !tbaa !12
  %13 = getelementptr inbounds i32, i32* %12, i64 10
  store volatile i32 %9, i32* %13, align 4, !tbaa !13
  %14 = tail call i32 @"\01_usleep"(i32 500000) #3
  %15 = load i32*, i32** getelementptr inbounds (%struct.rpi_gpio, %struct.rpi_gpio* @gpio, i64 0, i32 3), align 8, !tbaa !12
  %16 = getelementptr inbounds i32, i32* %15, i64 7
  store volatile i32 %9, i32* %16, align 4, !tbaa !13
  %17 = tail call i32 @"\01_usleep"(i32 500000) #3
  %18 = load i32*, i32** getelementptr inbounds (%struct.rpi_gpio, %struct.rpi_gpio* @gpio, i64 0, i32 3), align 8, !tbaa !12
  %19 = getelementptr inbounds i32, i32* %18, i64 10
  store volatile i32 %9, i32* %19, align 4, !tbaa !13
  %20 = tail call i32 @"\01_usleep"(i32 500000) #3
  %21 = load i32*, i32** getelementptr inbounds (%struct.rpi_gpio, %struct.rpi_gpio* @gpio, i64 0, i32 3), align 8, !tbaa !12
  %22 = getelementptr inbounds i32, i32* %21, i64 7
  store volatile i32 %9, i32* %22, align 4, !tbaa !13
  %23 = tail call i32 @"\01_usleep"(i32 500000) #3
  %24 = load i32*, i32** getelementptr inbounds (%struct.rpi_gpio, %struct.rpi_gpio* @gpio, i64 0, i32 3), align 8, !tbaa !12
  %25 = getelementptr inbounds i32, i32* %24, i64 10
  store volatile i32 %9, i32* %25, align 4, !tbaa !13
  %26 = tail call i32 @"\01_usleep"(i32 500000) #3
  %27 = load i32*, i32** getelementptr inbounds (%struct.rpi_gpio, %struct.rpi_gpio* @gpio, i64 0, i32 3), align 8, !tbaa !12
  %28 = getelementptr inbounds i32, i32* %27, i64 7
  store volatile i32 %9, i32* %28, align 4, !tbaa !13
  %29 = tail call i32 @"\01_usleep"(i32 500000) #3
  %30 = load i32*, i32** getelementptr inbounds (%struct.rpi_gpio, %struct.rpi_gpio* @gpio, i64 0, i32 3), align 8, !tbaa !12
  %31 = getelementptr inbounds i32, i32* %30, i64 10
  store volatile i32 %9, i32* %31, align 4, !tbaa !13
  %32 = tail call i32 @"\01_usleep"(i32 500000) #3
  %33 = load i32*, i32** getelementptr inbounds (%struct.rpi_gpio, %struct.rpi_gpio* @gpio, i64 0, i32 3), align 8, !tbaa !12
  %34 = getelementptr inbounds i32, i32* %33, i64 %7
  store volatile i32 0, i32* %34, align 4, !tbaa !13
  ret void
}

; Function Attrs: nounwind ssp uwtable
define i32 @GPIOsetup() local_unnamed_addr #0 {
  %1 = tail call i32 (i8*, i32, ...) @"\01_open"(i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str.1, i64 0, i64 0), i32 130) #3
  store i32 %1, i32* getelementptr inbounds (%struct.rpi_gpio, %struct.rpi_gpio* @gpio, i64 0, i32 1), align 8, !tbaa !3
  %2 = icmp slt i32 %1, 0
  br i1 %2, label %3, label %4

; <label>:3:                                      ; preds = %0
  tail call void @perror(i8* getelementptr inbounds ([48 x i8], [48 x i8]* @.str.2, i64 0, i64 0)) #5
  br label %10

; <label>:4:                                      ; preds = %0
  %5 = load i64, i64* getelementptr inbounds (%struct.rpi_gpio, %struct.rpi_gpio* @gpio, i64 0, i32 0), align 8, !tbaa !10
  %6 = tail call i8* @"\01_mmap"(i8* null, i64 4096, i32 3, i32 1, i32 %1, i64 %5) #3
  store i8* %6, i8** getelementptr inbounds (%struct.rpi_gpio, %struct.rpi_gpio* @gpio, i64 0, i32 2), align 8, !tbaa !11
  %7 = icmp eq i8* %6, inttoptr (i64 -1 to i8*)
  br i1 %7, label %8, label %9

; <label>:8:                                      ; preds = %4
  tail call void @perror(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @.str.3, i64 0, i64 0)) #5
  br label %10

; <label>:9:                                      ; preds = %4
  store i8* %6, i8** bitcast (i32** getelementptr inbounds (%struct.rpi_gpio, %struct.rpi_gpio* @gpio, i64 0, i32 3) to i8**), align 8, !tbaa !12
  br label %12

; <label>:10:                                     ; preds = %8, %3
  %11 = tail call i32 @puts(i8* getelementptr inbounds ([21 x i8], [21 x i8]* @str, i64 0, i64 0))
  br label %12

; <label>:12:                                     ; preds = %10, %9
  %13 = phi i32 [ 0, %9 ], [ 1, %10 ]
  ret i32 %13
}

; Function Attrs: nounwind
declare i32 @puts(i8* nocapture readonly) local_unnamed_addr #3

declare i1 @sleep(i32)

define i32 @main() {
entry:
  %array = alloca [2 x double]*, i32 2
  %k = alloca i1
  store i1 false, i1* %k
  %a = alloca i32
  store i32 32, i32* %a
  br i1 false, label %then, label %else

then:                                             ; preds = %entry
  %var_tmp = load i32, i32* %a
  %call_tmp = call i32 @printnum(i32 %var_tmp)
  br label %ifcont

else:                                             ; preds = %entry
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  ret i32 0
}

attributes #0 = { nounwind ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind }
attributes #4 = { cold }
attributes #5 = { cold nounwind }

!llvm.ident = !{!0}
!llvm.module.flags = !{!1, !2}

!0 = !{!"Apple LLVM version 10.0.0 (clang-1000.10.44.4)"}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{i32 7, !"PIC Level", i32 2}
!3 = !{!4, !8, i64 8}
!4 = !{!"", !5, i64 0, !8, i64 8, !9, i64 16, !9, i64 24}
!5 = !{!"long", !6, i64 0}
!6 = !{!"omnipotent char", !7, i64 0}
!7 = !{!"Simple C/C++ TBAA"}
!8 = !{!"int", !6, i64 0}
!9 = !{!"any pointer", !6, i64 0}
!10 = !{!4, !5, i64 0}
!11 = !{!4, !9, i64 16}
!12 = !{!4, !9, i64 24}
!13 = !{!8, !8, i64 0}
