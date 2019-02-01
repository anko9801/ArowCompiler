; ModuleID = 'on.arow'
source_filename = "on.arow"

declare i1 @Start()

declare i1 @End()

declare i1 @gpioMode(i32, i32)

declare i1 @gpioWrite(i32, i32)

declare i1 @gpioRead(i32)

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

define i32 @main() {
entry:
  %i = alloca i32
  store i32 0, i32* %i
  %call_tmp = call i1 @Start()
  %call_tmp1 = call i1 @gpioMode(i32 23, i32 0)
  %call_tmp2 = call i1 @gpioWrite(i32 23, i32 1)
  %call_tmp3 = call i1 @wait(i32 1000000)
  %call_tmp4 = call i1 @gpioWrite(i32 23, i32 0)
  %call_tmp5 = call i1 @gpioRead(i32 2)
  %0 = zext i1 %call_tmp5 to i32
  %call_tmp6 = call i1 @printnum(i32 %0)
  %call_tmp7 = call i1 @gpioRead(i32 3)
  %1 = zext i1 %call_tmp7 to i32
  %call_tmp8 = call i1 @printnum(i32 %1)
  %call_tmp9 = call i1 @End()
  %var_tmp = load i32, i32* %i
  ret i32 %var_tmp
}
