; ModuleID = 'test9.arow'
source_filename = "test9.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i32 @main() {
entry:
  %ifcond = icmp slt i32 13, 20
  br i1 %ifcond, label %loop, label %afterloop

loop:                                             ; preds = %loop, %entry
  %cnt.0 = phi i32 [ 13, %entry ], [ %add_tmp, %loop ]
  %add_tmp = add i32 %cnt.0, 1
  %ifcond3 = icmp slt i32 %add_tmp, 20
  br i1 %ifcond3, label %loop, label %afterloop

afterloop:                                        ; preds = %loop, %entry
  %cnt.1 = phi i32 [ %add_tmp, %loop ], [ 13, %entry ]
  %ifcond5 = icmp ne i32 %cnt.1, 0
  br i1 %ifcond5, label %then, label %else

then:                                             ; preds = %afterloop
  br label %ifcont

else:                                             ; preds = %afterloop
  ret i32 0
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %call_tmp = call i1 @printnum(i32 %cnt.1)
  ret i32 %cnt.1
}
