; ModuleID = 'test7.arow'
source_filename = "test7.arow"

declare i1 @printnum(i32)

declare i1 @printr(i32)

declare i1 @wait(i32)

declare i32 @usclock()

declare i32 @getPID()

define i1 @sleep(i32 %i_arg) {
entry:
  %i = alloca i32
  store i32 %i_arg, i32* %i
  %var_tmp = load i32, i32* %i
  %mul_tmp = mul i32 %var_tmp, 1000
  %call_tmp = call i1 @wait(i32 %mul_tmp)
  ret i1 false
}

define i32 @main() {
entry:
  %a = alloca i32
  store i32 4, i32* %a
  %b = alloca i32
  store i32 5, i32* %b
  %var_tmp = load i32, i32* %a
  %var_tmp1 = load i32, i32* %a
  %ifcond = icmp eq i32 %var_tmp1, 4
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %entry
  %var_tmp2 = load i32, i32* %a
  %var_tmp3 = load i32, i32* %b
  %add_tmp = add i32 %var_tmp2, %var_tmp3
  %call_tmp = call i1 @printnum(i32 %add_tmp)
  %var_tmp4 = load i32, i32* %a
  %var_tmp5 = load i32, i32* %b
  %sub_tmp = sub i32 %var_tmp4, %var_tmp5
  %call_tmp6 = call i1 @printnum(i32 %sub_tmp)
  br label %ifcont

else:                                             ; preds = %entry
  %var_tmp7 = load i32, i32* %a
  %var_tmp8 = load i32, i32* %b
  %mul_tmp = mul i32 %var_tmp7, %var_tmp8
  %call_tmp9 = call i1 @printnum(i32 %mul_tmp)
  %var_tmp10 = load i32, i32* %a
  %var_tmp11 = load i32, i32* %b
  %div_tmp = sdiv i32 %var_tmp10, %var_tmp11
  %call_tmp12 = call i1 @printnum(i32 %div_tmp)
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  ret i32 0
}
