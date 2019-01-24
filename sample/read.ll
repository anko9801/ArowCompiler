; ModuleID = 'read.arow'
source_filename = "read.arow"

declare i1 @printnum(i32)

declare i1 @printr(i32)

declare i1 @wait(i32)

declare i32 @usclock()

declare i1 @confirm()

declare i1 @Close()

declare i1 @gpioMode(i32, i32)

declare i1 @gpioWrite(i32, i32)

declare i1 @gpioRead(i32)

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
  %call_tmp = call i1 @confirm()
  %call_tmp1 = call i1 @gpioMode(i32 23, i32 1)
  %i = alloca i32
  store i32 0, i32* %i
  %call_tmp2 = call i1 @gpioRead(i32 23)
  %0 = zext i1 %call_tmp2 to i32
  %add_tmp = add i32 230, %0
  %call_tmp3 = call i1 @printnum(i32 %add_tmp)
  %var_tmp = load i32, i32* %i
  %var_tmp4 = load i32, i32* %i
  %ifcond = icmp slt i32 %var_tmp4, 10000
  br i1 %ifcond, label %loop, label %afterloop

loop:                                             ; preds = %loop, %entry
  %var_tmp5 = load i32, i32* %i
  %add_tmp6 = add i32 %var_tmp5, 1
  store i32 %add_tmp6, i32* %i
  %call_tmp7 = call i1 @gpioRead(i32 23)
  %1 = zext i1 %call_tmp7 to i32
  %add_tmp8 = add i32 230, %1
  %call_tmp9 = call i1 @printr(i32 %add_tmp8)
  %call_tmp10 = call i1 @wait(i32 10000)
  %var_tmp11 = load i32, i32* %i
  %var_tmp12 = load i32, i32* %i
  %ifcond13 = icmp slt i32 %var_tmp12, 10000
  br i1 %ifcond13, label %loop, label %afterloop

afterloop:                                        ; preds = %loop, %entry
  %call_tmp14 = call i1 @Close()
  ret i32 0
}
