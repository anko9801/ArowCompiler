; ModuleID = 'test10.arow'
source_filename = "test10.arow"

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
  %ifcond = icmp slt i32 0, 5
  br i1 %ifcond, label %loop, label %afterloop

loop:                                             ; preds = %loop, %entry
  %cnt.0 = phi i32 [ 0, %entry ], [ %add_tmp, %loop ]
  %call_tmp = call i1 @sleep(i32 2)
  %add_tmp = add i32 %cnt.0, 1
  %call_tmp4 = call i1 @printnum(i32 %add_tmp)
  %ifcond7 = icmp slt i32 %add_tmp, 5
  br i1 %ifcond7, label %loop, label %afterloop

afterloop:                                        ; preds = %loop, %entry
  %cnt.1 = phi i32 [ %add_tmp, %loop ], [ 0, %entry ]
  ret i32 %cnt.1
}
