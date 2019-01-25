; ModuleID = 'match.arow'
source_filename = "match.arow"

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
  %a = alloca i32
  store i32 5, i32* %a
  %var_tmp = load i32, i32* %a
  %var_tmp1 = load i32, i32* %a
  %ifcond = icmp eq i32 %var_tmp1, 5
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %entry
  store i32 2, i32* %a
  br label %ifcont12

else:                                             ; preds = %entry
  %var_tmp2 = load i32, i32* %a
  br i1 true, label %then3, label %else10

then3:                                            ; preds = %else
  %var_tmp4 = load i32, i32* %a
  %var_tmp5 = load i32, i32* %a
  %ifcond6 = icmp ne i32 %var_tmp5, 0
  br i1 %ifcond6, label %then7, label %else8

then7:                                            ; preds = %then3
  br label %ifcont

else8:                                            ; preds = %then3
  ret i32 0
  br label %ifcont

ifcont:                                           ; preds = %else8, %then7
  %var_tmp9 = load i32, i32* %a
  %call_tmp = call i1 @printnum(i32 %var_tmp9)
  br label %ifcont11

else10:                                           ; preds = %else
  br label %ifcont11

ifcont11:                                         ; preds = %else10, %ifcont
  br label %ifcont12

ifcont12:                                         ; preds = %ifcont11, %then
  %var_tmp13 = load i32, i32* %a
  %var_tmp14 = load i32, i32* %a
  %ifcond15 = icmp ne i32 %var_tmp14, 0
  br i1 %ifcond15, label %then16, label %else17

then16:                                           ; preds = %ifcont12
  br label %ifcont18

else17:                                           ; preds = %ifcont12
  ret i32 0
  br label %ifcont18

ifcont18:                                         ; preds = %else17, %then16
  %var_tmp19 = load i32, i32* %a
  %call_tmp20 = call i1 @printnum(i32 %var_tmp19)
  ret i32 0
}
