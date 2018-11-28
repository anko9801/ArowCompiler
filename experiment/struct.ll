; ModuleID = 'struct.c'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.a = type { i32, i8 }

define i32 @main() nounwind uwtable {
entry:
  %retval = alloca i32, align 4
  %A = alloca %struct.a, align 4
  store i32 0, i32* %retval
  %size = getelementptr inbounds %struct.a* %A, i32 0, i32 0
  store i32 3, i32* %size, align 4
  %size1 = getelementptr inbounds %struct.a* %A, i32 0, i32 0
  %0 = load i32* %size1, align 4
  ret i32 %0
}
