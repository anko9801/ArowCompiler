; ModuleID = 'array.c'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@main.a = private unnamed_addr constant [3 x i32] [i32 12, i32 232, i32 3434], align 4
@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1

define i32 @main() nounwind uwtable {
entry:
  %retval = alloca i32, align 4
  %a = alloca [3 x i32], align 4
  %b = alloca i32, align 4
  store i32 0, i32* %retval
  %0 = bitcast [3 x i32]* %a to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %0, i8* bitcast ([3 x i32]* @main.a to i8*), i64 12, i32 4, i1 false)
  store i32 0, i32* %b, align 4
  %arrayidx = getelementptr inbounds [3 x i32]* %a, i32 0, i64 1
  store i32 34, i32* %arrayidx, align 4
  %arrayidx1 = getelementptr inbounds [3 x i32]* %a, i32 0, i64 1
  %1 = load i32* %arrayidx1, align 4
  %call = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([3 x i8]* @.str, i32 0, i32 0), i32 %1)
  %arrayidx2 = getelementptr inbounds [3 x i32]* %a, i32 0, i64 2
  store i32 2, i32* %arrayidx2, align 4
  %arrayidx3 = getelementptr inbounds [3 x i32]* %a, i32 0, i64 2
  %2 = load i32* %arrayidx3, align 4
  ret i32 %2
}

declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture, i8* nocapture, i64, i32, i1) nounwind

declare i32 @printf(i8*, ...)
