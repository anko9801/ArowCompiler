; ModuleID = 'test6.ll'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

declare i32 @sleep(i32)

define i32 @main() {
entry:
  br i1 true, label %then, label %else

then:                                             ; preds = %entry
  %add_tmp = add i32 4, 5
  %call_tmp = call i32 @printnum(i32 %add_tmp)
  %sub_tmp = sub i32 4, 5
  %call_tmp4 = call i32 @printnum(i32 %sub_tmp)
  br label %ifcont

else:                                             ; preds = %entry
  %ak = alloca i32
  store i32 4, i32* %ak
  %bk = alloca i32
  store i32 5, i32* %bk
  %var_tmp7 = load i32* %ak
  %var_tmp8 = load i32* %bk
  %mul_tmp = mul i32 %var_tmp7, %var_tmp8
  %call_tmp9 = call i32 @printnum(i32 %mul_tmp)
  %var_tmp10 = load i32* %ak
  %var_tmp11 = load i32* %bk
  %div_tmp = sdiv i32 %var_tmp10, %var_tmp11
  %call_tmp12 = call i32 @printnum(i32 %div_tmp)
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  ret i32 0
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
