; ModuleID = 'test7.arow'
source_filename = "test7.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i32 @main() {
entry:
  %ifcond = icmp eq i32 4, 4
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %entry
  %add_tmp = add i32 4, 5
  %call_tmp = call i1 @printnum(i32 %add_tmp)
  %sub_tmp = sub i32 4, 5
  %call_tmp6 = call i1 @printnum(i32 %sub_tmp)
  br label %ifcont

else:                                             ; preds = %entry
  %mul_tmp = mul i32 4, 5
  %call_tmp9 = call i1 @printnum(i32 %mul_tmp)
  %div_tmp = sdiv i32 4, 5
  %call_tmp12 = call i1 @printnum(i32 %div_tmp)
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  ret i32 0
}
