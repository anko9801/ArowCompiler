; ModuleID = 'test3.arow'
source_filename = "test3.arow"

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
  %k = alloca i32
  store i32 23, i32* %k
  %var_tmp = load i32, i32* %k
  %var_tmp1 = load i32, i32* %k
  %ifcond = icmp ne i32 %var_tmp1, 0
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %entry
  br label %ifcont

else:                                             ; preds = %entry
  ret i32 0
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %var_tmp2 = load i32, i32* %k
  %call_tmp = call i1 @printnum(i32 %var_tmp2)
  ret i32 0
}
