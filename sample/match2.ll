; ModuleID = 'match2.arow'
source_filename = "match2.arow"

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
  store i32 5, i32* %a
  %var_tmp = load i32, i32* %a
  %var_tmp1 = load i32, i32* %a
  %ifcond = icmp eq i32 %var_tmp1, 5
  br i1 %ifcond, label %then, label %else7

then:                                             ; preds = %entry
  %var_tmp2 = load i32, i32* %a
  %var_tmp3 = load i32, i32* %a
  %ifcond4 = icmp ne i32 %var_tmp3, 0
  br i1 %ifcond4, label %then5, label %else

then5:                                            ; preds = %then
  br label %ifcont

else:                                             ; preds = %then
  ret i32 0
  br label %ifcont

ifcont:                                           ; preds = %else, %then5
  %var_tmp6 = load i32, i32* %a
  %call_tmp = call i1 @printnum(i32 %var_tmp6)
  br label %ifcont8

else7:                                            ; preds = %entry
  br label %ifcont8

ifcont8:                                          ; preds = %else7, %ifcont
  %var_tmp9 = load i32, i32* %a
  ret i32 %var_tmp9
}
