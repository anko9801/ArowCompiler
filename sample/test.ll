; ModuleID = 'test.arow'
source_filename = "test.arow"

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

define i32 @test(i32 %j_arg) {
entry:
  %j = alloca i32
  store i32 %j_arg, i32* %j
  %i = alloca i32
  %var_tmp = load i32, i32* %j
  %mul_tmp = mul i32 %var_tmp, 10
  store i32 %mul_tmp, i32* %i
  %var_tmp1 = load i32, i32* %i
  ret i32 %var_tmp1
}

define i32 @main() {
entry:
  %i = alloca i32
  store i32 10, i32* %i
  %var_tmp = load i32, i32* %i
  %call_tmp = call i32 @test(i32 %var_tmp)
  %var_tmp1 = load i32, i32* %i
  %call_tmp2 = call i32 @test(i32 %var_tmp1)
  %ifcond = icmp ne i32 %call_tmp2, 0
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %entry
  br label %ifcont

else:                                             ; preds = %entry
  ret i32 0
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %var_tmp3 = load i32, i32* %i
  %call_tmp4 = call i32 @test(i32 %var_tmp3)
  %call_tmp5 = call i1 @printnum(i32 %call_tmp4)
  ret i32 0
}
