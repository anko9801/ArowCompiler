; ModuleID = 'while.arow'
source_filename = "while.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i32 @main() {
entry:
  %ifcond = icmp slt i32 0, 5
  br i1 %ifcond, label %loop, label %afterloop

loop:                                             ; preds = %loop, %entry
  %i.0 = phi i32 [ 0, %entry ], [ %add_tmp, %loop ]
  %add_tmp = add i32 %i.0, 1
  %call_tmp = call i1 @printnum(i32 %add_tmp)
  %ifcond6 = icmp slt i32 %add_tmp, 5
  br i1 %ifcond6, label %loop, label %afterloop

afterloop:                                        ; preds = %loop, %entry
  ret i32 0
}
