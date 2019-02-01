; ModuleID = 'demo1.arow'
source_filename = "demo1.arow"

declare i1 @printnum(i32)

declare i1 @printr(i32)

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
  %call_tmp2 = call i1 @gpioMode(i32 22, i32 0)
  %i = alloca i32
  store i32 0, i32* %i
  %call_tmp3 = call i1 @gpioRead(i32 23)
  %0 = zext i1 %call_tmp3 to i32
  %add_tmp = add i32 230, %0
  %call_tmp4 = call i1 @printnum(i32 %add_tmp)
  %var_tmp = load i32, i32* %i
  %var_tmp5 = load i32, i32* %i
  %ifcond = icmp slt i32 %var_tmp5, 100000
  br i1 %ifcond, label %loop, label %afterloop

loop:                                             ; preds = %ifcont, %entry
  %call_tmp6 = call i1 @gpioRead(i32 23)
  %1 = zext i1 %call_tmp6 to i32
  %add_tmp7 = add i32 230, %1
  %call_tmp8 = call i1 @printr(i32 %add_tmp7)
  %call_tmp9 = call i1 @gpioRead(i32 23)
  %call_tmp10 = call i1 @gpioRead(i32 23)
  %ifcond11 = icmp eq i1 %call_tmp10, true
  br i1 %ifcond11, label %then, label %else

then:                                             ; preds = %loop
  %call_tmp12 = call i1 @gpioWrite(i32 22, i32 0)
  br label %ifcont

else:                                             ; preds = %loop
  %call_tmp13 = call i1 @gpioWrite(i32 22, i32 1)
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %call_tmp14 = call i1 @wait(i32 5000)
  %var_tmp15 = load i32, i32* %i
  %add_tmp16 = add i32 %var_tmp15, 1
  store i32 %add_tmp16, i32* %i
  %var_tmp17 = load i32, i32* %i
  %var_tmp18 = load i32, i32* %i
  %ifcond19 = icmp slt i32 %var_tmp18, 100000
  br i1 %ifcond19, label %loop, label %afterloop

afterloop:                                        ; preds = %ifcont, %entry
  %call_tmp20 = call i1 @End()
  ret i32 0
}
