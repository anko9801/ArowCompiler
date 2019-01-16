; ModuleID = 'if2.arow'
source_filename = "if2.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i32 @main() {
entry:
  %ifcond = icmp sgt i32 2, 0
  br i1 %ifcond, label %then, label %else7

then:                                             ; preds = %entry
  %ifcond4 = icmp slt i32 2, 10
  br i1 %ifcond4, label %then5, label %else

then5:                                            ; preds = %then
  %call_tmp = call i1 @printnum(i32 2)
  br label %ifcont

else:                                             ; preds = %then
  br label %ifcont

ifcont:                                           ; preds = %else, %then5
  br label %ifcont8

else7:                                            ; preds = %entry
  br label %ifcont8

ifcont8:                                          ; preds = %else7, %ifcont
  ret i32 2
}
