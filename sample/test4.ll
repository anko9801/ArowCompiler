; ModuleID = 'test4.arow'
source_filename = "test4.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i32 @main() {
entry:
  %ifcond = icmp ne i32 23, 0
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %entry
  br label %ifcont

else:                                             ; preds = %entry
  ret i32 0
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %call_tmp = call i1 @printnum(i32 23)
  ret i32 0
}
