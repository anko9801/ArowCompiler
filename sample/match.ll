; ModuleID = 'match.arow'
source_filename = "match.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i1 @sleep(i32 %i_arg) {
entry:
  %mul_tmp = mul i32 %i_arg, 1000
  %call_tmp = call i1 @wait(i32 %mul_tmp)
  ret i1 false
}

define i32 @main() {
entry:
  %ifcond = icmp eq i32 5, 5
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %entry
  br label %ifcont6

else:                                             ; preds = %entry
  br i1 true, label %then3, label %else5

then3:                                            ; preds = %else
  %call_tmp = call i1 @printnum(i32 5)
  br label %ifcont

else5:                                            ; preds = %else
  br label %ifcont

ifcont:                                           ; preds = %else5, %then3
  br label %ifcont6

ifcont6:                                          ; preds = %ifcont, %then
  %a.0 = phi i32 [ 2, %then ], [ 5, %ifcont ]
  %call_tmp8 = call i1 @printnum(i32 %a.0)
  ret i32 0
}
