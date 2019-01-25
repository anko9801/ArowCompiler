; ModuleID = 'benchmark.arow'
source_filename = "benchmark.arow"

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
  %start = alloca i32
  %call_tmp = call i32 @usclock()
  store i32 %call_tmp, i32* %start
  %var_tmp = load i32, i32* %i
  %var_tmp1 = load i32, i32* %i
  %ifcond = icmp slt i32 %var_tmp1, 1000000
  br i1 %ifcond, label %loop, label %afterloop

loop:                                             ; preds = %ifcont, %entry
  %var_tmp2 = load i32, i32* %i
  %add_tmp = add i32 %var_tmp2, 1
  store i32 %add_tmp, i32* %i
  %var_tmp3 = load i32, i32* %i
  %var_tmp4 = load i32, i32* %i
  %ifcond5 = icmp ne i32 %var_tmp4, 0
  br i1 %ifcond5, label %then, label %else

then:                                             ; preds = %loop
  br label %ifcont

else:                                             ; preds = %loop
  ret i32 0
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %var_tmp6 = load i32, i32* %i
  %call_tmp7 = call i1 @printnum(i32 %var_tmp6)
  %var_tmp8 = load i32, i32* %i
  %var_tmp9 = load i32, i32* %i
  %ifcond10 = icmp slt i32 %var_tmp9, 1000000
  br i1 %ifcond10, label %loop, label %afterloop

afterloop:                                        ; preds = %ifcont, %entry
  %var_tmp11 = load i32, i32* %start
  %var_tmp12 = load i32, i32* %start
  %ifcond13 = icmp ne i32 %var_tmp12, 0
  br i1 %ifcond13, label %then14, label %else15

then14:                                           ; preds = %afterloop
  br label %ifcont16

else15:                                           ; preds = %afterloop
  ret i32 0
  br label %ifcont16

ifcont16:                                         ; preds = %else15, %then14
  %call_tmp17 = call i32 @usclock()
  %var_tmp18 = load i32, i32* %start
  %sub_tmp = sub i32 %call_tmp17, %var_tmp18
  %call_tmp19 = call i1 @printnum(i32 %sub_tmp)
  %var_tmp20 = load i32, i32* %i
  ret i32 %var_tmp20
}
