; ModuleID = 'test7.arow'
source_filename = "test7.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i32 @main() {
entry:
  %array = alloca [2 x double]*, i32 2
  %a = alloca i32
  store i32 4, i32* %a
  %b = alloca i32
  store i32 5, i32* %b
  %var_tmp = load i32, i32* %a
  %ifcond = icmp eq i32 %var_tmp, 4
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %entry
  %var_tmp1 = load i32, i32* %a
  %var_tmp2 = load i32, i32* %b
  %add_tmp = add i32 %var_tmp1, %var_tmp2
  %call_tmp = call i1 @printnum(i32 %add_tmp)
  %var_tmp3 = load i32, i32* %a
  %var_tmp4 = load i32, i32* %b
  %sub_tmp = sub i32 %var_tmp3, %var_tmp4
  %call_tmp5 = call i1 @printnum(i32 %sub_tmp)
  br label %ifcont

else:                                             ; preds = %entry
  %var_tmp6 = load i32, i32* %a
  %var_tmp7 = load i32, i32* %b
  %mul_tmp = mul i32 %var_tmp6, %var_tmp7
  %call_tmp8 = call i1 @printnum(i32 %mul_tmp)
  %var_tmp9 = load i32, i32* %a
  %var_tmp10 = load i32, i32* %b
  %div_tmp = sdiv i32 %var_tmp9, %var_tmp10
  %call_tmp11 = call i1 @printnum(i32 %div_tmp)
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  ret i32 0
}
