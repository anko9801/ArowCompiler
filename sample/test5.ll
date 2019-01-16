; ModuleID = 'test5.arow'
source_filename = "test5.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i32 @main() {
entry:
  br i1 false, label %then, label %else

then:                                             ; preds = %entry
  %call_tmp = call i1 @printnum(i32 32)
  br label %ifcont

else:                                             ; preds = %entry
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  ret i32 0
}
