; ModuleID = 'benchmark.arow'
source_filename = "benchmark.arow"

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

define i32 @fibo(i6 %x_arg) {
entry:
  %x = alloca i6
  store i6 %x_arg, i6* %x
  %var_tmp = load i6, i6* %x
  %var_tmp1 = load i6, i6* %x
  %ifcond = icmp eq i6 %var_tmp1, 0
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %entry
  ret i32 0
  br label %ifcont15

else:                                             ; preds = %entry
  %var_tmp2 = load i6, i6* %x
  %var_tmp3 = load i6, i6* %x
  %ifcond4 = icmp eq i6 %var_tmp3, 1
  br i1 %ifcond4, label %then5, label %else6

then5:                                            ; preds = %else
  ret i32 1
  br label %ifcont14

else6:                                            ; preds = %else
  %var_tmp7 = load i6, i6* %x
  br i1 true, label %then8, label %else13

then8:                                            ; preds = %else6
  %var_tmp9 = load i6, i6* %x
  %sub_tmp = sub i6 %var_tmp9, 1
  %call_tmp = call i32 @fibo(i6 %sub_tmp)
  %var_tmp10 = load i6, i6* %x
  %sub_tmp11 = sub i6 %var_tmp10, 2
  %call_tmp12 = call i32 @fibo(i6 %sub_tmp11)
  %add_tmp = add i32 %call_tmp, %call_tmp12
  ret i32 %add_tmp
  br label %ifcont

else13:                                           ; preds = %else6
  br label %ifcont

ifcont:                                           ; preds = %else13, %then8
  br label %ifcont14

ifcont14:                                         ; preds = %ifcont, %then5
  br label %ifcont15

ifcont15:                                         ; preds = %ifcont14, %then
  ret i32 0
}

define i32 @main() {
entry:
  %call_tmp = call i32 @getPID()
  %call_tmp1 = call i1 @printnum(i32 %call_tmp)
  %start = alloca i32
  %call_tmp2 = call i32 @usclock()
  store i32 %call_tmp2, i32* %start
  %call_tmp3 = call i32 @fibo(i6 -19)
  %call_tmp4 = call i1 @printnum(i32 %call_tmp3)
  %call_tmp5 = call i32 @usclock()
  %var_tmp = load i32, i32* %start
  %sub_tmp = sub i32 %call_tmp5, %var_tmp
  %call_tmp6 = call i1 @printnum(i32 %sub_tmp)
  ret i32 0
}
