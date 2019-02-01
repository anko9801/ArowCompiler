; ModuleID = 'if2.arow'
source_filename = "if2.arow"

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
  store i32 2, i32* %a
  %var_tmp = load i32, i32* %a
  %var_tmp1 = load i32, i32* %a
  %ifcond = icmp sgt i32 %var_tmp1, 0
  br i1 %ifcond, label %then, label %else13

then:                                             ; preds = %entry
  %var_tmp2 = load i32, i32* %a
  %var_tmp3 = load i32, i32* %a
  %ifcond4 = icmp slt i32 %var_tmp3, 10
  br i1 %ifcond4, label %then5, label %else11

then5:                                            ; preds = %then
  %var_tmp6 = load i32, i32* %a
  %var_tmp7 = load i32, i32* %a
  %ifcond8 = icmp ne i32 %var_tmp7, 0
  br i1 %ifcond8, label %then9, label %else

then9:                                            ; preds = %then5
  br label %ifcont

else:                                             ; preds = %then5
  ret i32 0
  br label %ifcont

ifcont:                                           ; preds = %else, %then9
  %var_tmp10 = load i32, i32* %a
  %call_tmp = call i1 @printnum(i32 %var_tmp10)
  br label %ifcont12

else11:                                           ; preds = %then
  br label %ifcont12

ifcont12:                                         ; preds = %else11, %ifcont
  br label %ifcont14

else13:                                           ; preds = %entry
  br label %ifcont14

ifcont14:                                         ; preds = %else13, %ifcont12
  %var_tmp15 = load i32, i32* %a
  ret i32 %var_tmp15
}
