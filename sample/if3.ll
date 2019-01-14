; ModuleID = 'if3.arow'
source_filename = "if3.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i32 @main() {
entry:
  %ifcond = icmp ne i32 0, 0
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %entry
  br label %ifcont

else:                                             ; preds = %entry
  ret i32 0
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %ifcond2 = icmp eq i32 0, 5
  %ifcond4 = icmp sgt i32 0, -1
  %and_cond = and i1 %ifcond2, %ifcond4
  br i1 %and_cond, label %then5, label %else7

then5:                                            ; preds = %ifcont
  %call_tmp = call i1 @printnum(i32 0)
  br label %ifcont8

else7:                                            ; preds = %ifcont
  br label %ifcont8

ifcont8:                                          ; preds = %else7, %then5
  ret i32 0
}
