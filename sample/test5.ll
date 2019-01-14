; ModuleID = 'test5.arow'
source_filename = "test5.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i32 @main() {
entry:
  %ifcond = icmp ne i32 32, 0
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %entry
  br label %ifcont

else:                                             ; preds = %entry
  ret i32 0
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  br i1 false, label %then1, label %else3

then1:                                            ; preds = %ifcont
  %call_tmp = call i1 @printnum(i32 32)
  br label %ifcont4

else3:                                            ; preds = %ifcont
  br label %ifcont4

ifcont4:                                          ; preds = %else3, %then1
  ret i32 0
}
