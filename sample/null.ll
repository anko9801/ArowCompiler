; ModuleID = 'null.arow'
source_filename = "null.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i32 @none() {
entry:
  ret i32 0
}

define i32 @main() {
entry:
  %call_tmp = call i32 @none()
  %ifcond = icmp eq i32 %call_tmp, 0
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %entry
  %call_tmp2 = call i1 @printnum(i32 1)
  ret i32 %call_tmp
  br label %ifcont

else:                                             ; preds = %entry
  %call_tmp5 = call i1 @printnum(i32 %call_tmp)
  ret i32 %call_tmp
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  ret i32 %call_tmp
}
