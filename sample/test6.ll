; ModuleID = 'test6.arow'
source_filename = "test6.arow"

declare i1 @printnum(i32)

declare i1 @sleep(i32)

declare i32 @usclock()

declare i1 @BlinkLED(i32)

declare i1 @GPIOsetup()

declare i1 @GPIOclear()

define i32 @main() {
entry:
  br i1 false, label %then, label %else

then:                                             ; preds = %entry
  br label %ifcont

else:                                             ; preds = %entry
  %ak = alloca i32
  store i32 4, i32* %ak
  %bk = alloca i32
  store i32 5, i32* %bk
  %var_tmp2 = load i32, i32* %ak
  %var_tmp3 = load i32, i32* %bk
  %mul_tmp = mul i32 %var_tmp2, %var_tmp3
  %call_tmp = call i1 @printnum(i32 %mul_tmp)
  %var_tmp4 = load i32, i32* %ak
  %var_tmp5 = load i32, i32* %bk
  %div_tmp = sdiv i32 %var_tmp4, %var_tmp5
  %call_tmp6 = call i1 @printnum(i32 %div_tmp)
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  ret i32 0
}
