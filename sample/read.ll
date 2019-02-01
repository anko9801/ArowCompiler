; ModuleID = 'read.arow'
source_filename = "read.arow"

declare i1 @printnum(i32)

declare i1 @printr(i32)

declare i1 @print(i32)

declare i1 @wait(i32)

declare i32 @usclock()

declare i32 @getPID()

declare i1 @Start()

declare i1 @End()

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
  %call_tmp = call i1 @Start()
  %call_tmp1 = call i1 @gpioMode(i32 23, i32 1)
  %i = alloca i32
  store i32 0, i32* %i
  %call_tmp2 = call i1 @gpioRead(i32 23)
  %call_tmp3 = call i1 @gpioRead(i32 23)
  %ifcond = icmp ugt i1 false, %call_tmp3
  %0 = zext i1 %ifcond to i32
  %call_tmp4 = call i1 @printnum(i32 %0)
  %var_tmp = load i32, i32* %i
  %var_tmp5 = load i32, i32* %i
  %ifcond6 = icmp slt i32 %var_tmp5, 10000
  br i1 %ifcond6, label %loop, label %afterloop

loop:                                             ; preds = %loop, %entry
  %var_tmp7 = load i32, i32* %i
  %add_tmp = add i32 %var_tmp7, 1
  store i32 %add_tmp, i32* %i
  %call_tmp8 = call i1 @gpioRead(i32 23)
  %call_tmp9 = call i1 @gpioRead(i32 23)
  %ifcond10 = icmp ugt i1 false, %call_tmp9
  %1 = zext i1 %ifcond10 to i32
  %call_tmp11 = call i1 @printr(i32 %1)
  %call_tmp12 = call i1 @wait(i32 10000)
  %var_tmp13 = load i32, i32* %i
  %var_tmp14 = load i32, i32* %i
  %ifcond15 = icmp slt i32 %var_tmp14, 10000
  br i1 %ifcond15, label %loop, label %afterloop

afterloop:                                        ; preds = %loop, %entry
  %call_tmp16 = call i1 @End()
  ret i32 0
}
