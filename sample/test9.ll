; ModuleID = 'test9.ll'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

define i32 @main() {
entry:
  %ifcond = icmp slt i32 0, 5
  br label %loop

loop:                                             ; preds = %loop, %entry
  %cnt.0 = phi i32 [ 0, %entry ], [ %add_tmp, %loop ]
  %add_tmp = add i32 %cnt.0, 1
  %ifcond3 = icmp slt i32 %add_tmp, 5
  br i1 %ifcond3, label %loop, label %afterloop

afterloop:                                        ; preds = %loop
  %call_tmp = call i32 @printnum(i32 %add_tmp)
  ret i32 %add_tmp
}

define i32 @printnum(i32 %i) nounwind uwtable {
entry:
  %call = tail call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([4 x i8]* @.str, i64 0, i64 0), i32 %i) nounwind
  ret i32 %call
}

declare i32 @printf(i8* nocapture, ...) nounwind
