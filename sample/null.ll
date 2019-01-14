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
  br i1 %ifcond, label %then, label %else6

then:                                             ; preds = %entry
  %call_tmp1 = call i1 @printnum(i32 1)
  ret i32 %call_tmp
  %ifcond4 = icmp ne i32 %call_tmp, 0
  br i1 %ifcond4, label %then5, label %else

then5:                                            ; preds = %then
  br label %ifcont

else:                                             ; preds = %then
  ret i32 0
  br label %ifcont

ifcont:                                           ; preds = %else, %then5
  br label %ifcont10

else6:                                            ; preds = %entry
  %call_tmp8 = call i1 @printnum(i32 %call_tmp)
  ret i32 %call_tmp
  br label %ifcont10

ifcont10:                                         ; preds = %else6, %ifcont
  ret i32 %call_tmp
}
