; ModuleID = 'test6.arow'
source_filename = "test6.arow"

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
  br i1 false, label %then, label %else

then:                                             ; preds = %entry
  %var_tmp = load i32, i32* %a
  %var_tmp1 = load i32, i32* %b
  %add_tmp = add i32 %var_tmp, %var_tmp1
  %call_tmp = call i1 @printnum(i32 %add_tmp)
  %var_tmp2 = load i32, i32* %a
  %var_tmp3 = load i32, i32* %b
  %sub_tmp = sub i32 %var_tmp2, %var_tmp3
  %call_tmp4 = call i1 @printnum(i32 %sub_tmp)
  br label %ifcont

else:                                             ; preds = %entry
  %ak = alloca i32
  %var_tmp5 = load i32, i32* %a
  store i32 %var_tmp5, i32* %ak
  %bk = alloca i32
  %var_tmp6 = load i32, i32* %b
  store i32 %var_tmp6, i32* %bk
  %var_tmp7 = load i32, i32* %ak
  %var_tmp8 = load i32, i32* %bk
  %mul_tmp = mul i32 %var_tmp7, %var_tmp8
  %call_tmp9 = call i1 @printnum(i32 %mul_tmp)
  %var_tmp10 = load i32, i32* %ak
  %var_tmp11 = load i32, i32* %bk
  %div_tmp = sdiv i32 %var_tmp10, %var_tmp11
  %call_tmp12 = call i1 @printnum(i32 %div_tmp)
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  ret i32 0
}
