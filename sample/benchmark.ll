; ModuleID = 'benchmark.ll'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

declare i32 @sleep(i32)

define i32 @main() {
entry:
  %call_tmp = call i32 @usclock()
  %ifcond = icmp slt i32 0, 1000000
  br label %loop

loop:                                             ; preds = %loop, %entry
  %i.0 = phi i32 [ 0, %entry ], [ %add_tmp, %loop ]
  %add_tmp = add i32 %i.0, 1
  %call_tmp3 = call i32 @printnum(i32 %add_tmp)
  %ifcond5 = icmp slt i32 %add_tmp, 1000000
  br i1 %ifcond5, label %loop, label %afterloop

afterloop:                                        ; preds = %loop
  %call_tmp6 = call i32 @usclock()
  %sub_tmp = sub i32 %call_tmp6, %call_tmp
  %call_tmp8 = call i32 @printnum(i32 %sub_tmp)
  ret i32 %add_tmp
}

define i32 @msleep(i32 %ms) nounwind uwtable {
entry:
  %ms.addr = alloca i32, align 4
  store i32 %ms, i32* %ms.addr, align 4
  %call = call i32 @usleep(i32 10)
  ret i32 0
}

declare i32 @usleep(i32)

define i32 @printnum(i32 %i) nounwind uwtable {
entry:
  %i.addr = alloca i32, align 4
  store i32 %i, i32* %i.addr, align 4
  %0 = load i32* %i.addr, align 4
  %call = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([4 x i8]* @.str, i32 0, i32 0), i32 %0)
  ret i32 %call
}

declare i32 @printf(i8*, ...)

define i32 @usclock() nounwind uwtable {
entry:
  %call = call i64 @clock() nounwind
  %conv = sitofp i64 %call to double
  %mul = fmul double %conv, 1.000000e+06
  %div = fdiv double %mul, 1.000000e+06
  %conv1 = fptosi double %div to i32
  ret i32 %conv1
}

declare i64 @clock() nounwind
