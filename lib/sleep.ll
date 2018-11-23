; ModuleID = 'sleep.c'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

define i32 @msleep(i32 %ms) nounwind uwtable {
entry:
  %ms.addr = alloca i32, align 4
  store i32 %ms, i32* %ms.addr, align 4
  %call = call i32 @usleep(i32 10)
  ret i32 0
}

declare i32 @usleep(i32)
