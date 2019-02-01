; ModuleID = 'blink.arow'
source_filename = "blink.arow"

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
  %call_tmp1 = call i1 @gpioMode(i32 23, i32 0)
  %i = alloca i32
  store i32 0, i32* %i
  %var_tmp = load i32, i32* %i
  %var_tmp2 = load i32, i32* %i
  %ifcond = icmp ne i32 %var_tmp2, 5
  br i1 %ifcond, label %loop, label %afterloop

loop:                                             ; preds = %loop, %entry
  %var_tmp3 = load i32, i32* %i
  %add_tmp = add i32 %var_tmp3, 1
  store i32 %add_tmp, i32* %i
  %call_tmp4 = call i1 @gpioWrite(i32 23, i32 1)
  %call_tmp5 = call i1 @sleep(i32 1000)
  %call_tmp6 = call i1 @gpioWrite(i32 23, i32 0)
  %call_tmp7 = call i1 @sleep(i32 1000)
  %var_tmp8 = load i32, i32* %i
  %var_tmp9 = load i32, i32* %i
  %ifcond10 = icmp ne i32 %var_tmp9, 5
  br i1 %ifcond10, label %loop, label %afterloop

afterloop:                                        ; preds = %loop, %entry
  %call_tmp11 = call i1 @End()
  ret i32 0
}
