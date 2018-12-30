; ModuleID = 'test9.arow'
source_filename = "test9.arow"

declare i1 @printnum(i32)

declare i1 @sleep(i32)

declare i32 @usclock()

declare i1 @BlinkLED(i32)

declare i1 @GPIOsetup()

declare i1 @GPIOclear()

define i32 @main() {
entry:
  %ifcond = icmp slt i32 13, 20
  br label %loop

loop:                                             ; preds = %loop, %entry
  %cnt.0 = phi i32 [ 13, %entry ], [ %add_tmp, %loop ]
  %add_tmp = add i32 %cnt.0, 1
  br i1 %ifcond, label %loop, label %afterloop

afterloop:                                        ; preds = %loop
  %call_tmp = call i1 @printnum(i32 %add_tmp)
  ret i32 %add_tmp
}
