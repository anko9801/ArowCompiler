; ModuleID = 'test10.arow'
source_filename = "test10.arow"

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

define i32 @main() {
entry:
  %cnt = alloca i32
  store i32 0, i32* %cnt
  %var_tmp = load i32, i32* %cnt
  %var_tmp1 = load i32, i32* %cnt
  %ifcond = icmp slt i32 %var_tmp1, 5
  br i1 %ifcond, label %loop, label %afterloop

loop:                                             ; preds = %ifcont, %entry
  %call_tmp = call i1 @sleep(i32 2)
  %var_tmp2 = load i32, i32* %cnt
  %add_tmp = add i32 %var_tmp2, 1
  store i32 %add_tmp, i32* %cnt
  %var_tmp3 = load i32, i32* %cnt
  %var_tmp4 = load i32, i32* %cnt
  %ifcond5 = icmp ne i32 %var_tmp4, 0
  br i1 %ifcond5, label %then, label %else

then:                                             ; preds = %loop
  br label %ifcont

else:                                             ; preds = %loop
  ret i32 0
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %var_tmp6 = load i32, i32* %cnt
  %call_tmp7 = call i1 @printnum(i32 %var_tmp6)
  %var_tmp8 = load i32, i32* %cnt
  %var_tmp9 = load i32, i32* %cnt
  %ifcond10 = icmp slt i32 %var_tmp9, 5
  br i1 %ifcond10, label %loop, label %afterloop

afterloop:                                        ; preds = %ifcont, %entry
  %var_tmp11 = load i32, i32* %cnt
  ret i32 %var_tmp11
}
