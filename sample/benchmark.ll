; ModuleID = 'benchmark.arow'
source_filename = "benchmark.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i1 @sleep(i32 %i_arg) {
entry:
  %mul_tmp = mul i32 %i_arg, 1000
  %call_tmp = call i1 @wait(i32 %mul_tmp)
  ret i1 false
}

define i32 @main() {
entry:
  %call_tmp = call i32 @usclock()
  %ifcond = icmp slt i32 0, 1000000
  br i1 %ifcond, label %loop, label %afterloop

loop:                                             ; preds = %loop, %entry
  %i.0 = phi i32 [ 0, %entry ], [ %add_tmp, %loop ]
  %add_tmp = add i32 %i.0, 1
  %call_tmp4 = call i1 @printnum(i32 %add_tmp)
  %ifcond7 = icmp slt i32 %add_tmp, 1000000
  br i1 %ifcond7, label %loop, label %afterloop

afterloop:                                        ; preds = %loop, %entry
  %i.1 = phi i32 [ %add_tmp, %loop ], [ 0, %entry ]
  %call_tmp8 = call i32 @usclock()
  %sub_tmp = sub i32 %call_tmp8, %call_tmp
  %call_tmp10 = call i1 @printnum(i32 %sub_tmp)
  ret i32 %i.1
}
