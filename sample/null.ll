; ModuleID = 'null.arow'
source_filename = "null.arow"

declare i1 @printnum(i32)

declare i1 @printr(i32)

declare i1 @print(i32)

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

define i32 @none() {
entry:
  ret i32 0
}

define i32 @main() {
entry:
  %ab = alloca i32
  %call_tmp = call i32 @none()
  store i32 %call_tmp, i32* %ab
  %var_tmp = load i32, i32* %ab
  %var_tmp1 = load i32, i32* %ab
  %ifcond = icmp eq i32 %var_tmp1, 0
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %entry
  %call_tmp2 = call i1 @printnum(i32 1)
  %var_tmp3 = load i32, i32* %ab
  ret i32 %var_tmp3
  br label %ifcont12

else:                                             ; preds = %entry
  %var_tmp4 = load i32, i32* %ab
  %var_tmp5 = load i32, i32* %ab
  %ifcond6 = icmp ne i32 %var_tmp5, 0
  br i1 %ifcond6, label %then7, label %else8

then7:                                            ; preds = %else
  br label %ifcont

else8:                                            ; preds = %else
  ret i32 0
  br label %ifcont

ifcont:                                           ; preds = %else8, %then7
  %var_tmp9 = load i32, i32* %ab
  %call_tmp10 = call i1 @printnum(i32 %var_tmp9)
  %var_tmp11 = load i32, i32* %ab
  ret i32 %var_tmp11
  br label %ifcont12

ifcont12:                                         ; preds = %ifcont, %then
  %var_tmp13 = load i32, i32* %ab
  ret i32 %var_tmp13
}
