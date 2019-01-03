; ModuleID = 'test7.arow'
source_filename = "test7.arow"

declare i1 @printnum(i32)

declare i1 @sleep(i32)

declare i32 @usclock()

declare i1 @BlinkLED(i32)

declare i1 @GPIOsetup()

declare i1 @GPIOclear()

define i32 @main() {
entry:
  %ifcond = icmp eq i32 4, 4
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %entry
  br label %ifcont

else:                                             ; preds = %entry
  %mul_tmp = mul i32 4, 5
  %call_tmp = call i1 @printnum(i32 %mul_tmp)
  %div_tmp = sdiv i32 4, 5
  %call_tmp5 = call i1 @printnum(i32 %div_tmp)
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  ret i32 0
}
