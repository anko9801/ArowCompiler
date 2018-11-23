; ModuleID = 'test.ll'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

declare i32 @sleep(i32)

declare i32 @unwrap(i32, i32)

define i32 @test(i32 %j_arg) {
entry:
  %mul_tmp = mul i32 %j_arg, 10
  ret i32 %mul_tmp
}

define i32 @main() {
entry:
  %call_tmp = call i32 @test(i32 10)
  %call_tmp1 = call i32 @printnum(i32 %call_tmp)
  ret i32 0
}

define i32 @printnum(i32 %i) nounwind uwtable {
entry:
  %call = tail call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([4 x i8]* @.str, i64 0, i64 0), i32 %i) nounwind
  ret i32 %call
}

declare i32 @printf(i8* nocapture, ...) nounwind

define i32 @msleep(i32 %ms) nounwind uwtable {
entry:
  %ms.addr = alloca i32, align 4
  store i32 %ms, i32* %ms.addr, align 4
  %call = call i32 @usleep(i32 10)
  ret i32 0
}

declare i32 @usleep(i32)
