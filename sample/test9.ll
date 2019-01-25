; ModuleID = 'test9.arow'
source_filename = "test9.arow"

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
  %cnt = alloca i32
  store i32 13, i32* %cnt
  %var_tmp = load i32, i32* %cnt
  %var_tmp1 = load i32, i32* %cnt
  %ifcond = icmp slt i32 %var_tmp1, 20
  br i1 %ifcond, label %loop, label %afterloop

loop:                                             ; preds = %loop, %entry
  %var_tmp2 = load i32, i32* %cnt
  %add_tmp = add i32 %var_tmp2, 1
  store i32 %add_tmp, i32* %cnt
  %var_tmp3 = load i32, i32* %cnt
  %var_tmp4 = load i32, i32* %cnt
  %ifcond5 = icmp slt i32 %var_tmp4, 20
  br i1 %ifcond5, label %loop, label %afterloop

afterloop:                                        ; preds = %loop, %entry
  %var_tmp6 = load i32, i32* %cnt
  %var_tmp7 = load i32, i32* %cnt
  %ifcond8 = icmp ne i32 %var_tmp7, 0
  br i1 %ifcond8, label %then, label %else

then:                                             ; preds = %afterloop
  br label %ifcont

else:                                             ; preds = %afterloop
  ret i32 0
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %var_tmp9 = load i32, i32* %cnt
  %call_tmp = call i1 @printnum(i32 %var_tmp9)
  %var_tmp10 = load i32, i32* %cnt
  ret i32 %var_tmp10
}
