; ModuleID = 'if3.arow'
source_filename = "if3.arow"

declare i1 @printnum(i32)

declare i1 @printr(i32)

declare i1 @wait(i32)

declare i32 @usclock()

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
  %i = alloca i32
  store i32 0, i32* %i
  %var_tmp = load i32, i32* %i
  %var_tmp1 = load i32, i32* %i
  %ifcond = icmp eq i32 %var_tmp1, 5
  %var_tmp2 = load i32, i32* %i
  %var_tmp3 = load i32, i32* %i
  %ifcond4 = icmp sgt i32 %var_tmp3, -1
  %and_cond = and i1 %ifcond, %ifcond4
  br i1 %and_cond, label %then, label %else10

then:                                             ; preds = %entry
  %var_tmp5 = load i32, i32* %i
  %var_tmp6 = load i32, i32* %i
  %ifcond7 = icmp ne i32 %var_tmp6, 0
  br i1 %ifcond7, label %then8, label %else

then8:                                            ; preds = %then
  br label %ifcont

else:                                             ; preds = %then
  ret i32 0
  br label %ifcont

ifcont:                                           ; preds = %else, %then8
  %var_tmp9 = load i32, i32* %i
  %call_tmp = call i1 @printnum(i32 %var_tmp9)
  br label %ifcont11

else10:                                           ; preds = %entry
  br label %ifcont11

ifcont11:                                         ; preds = %else10, %ifcont
  ret i32 0
}
