; ModuleID = 'off.arow'
source_filename = "off.arow"

declare i1 @Start()

declare i1 @End()

declare i1 @gpioMode(i32, i32)

declare i1 @gpioWrite(i32, i32)

declare i1 @gpioRead(i32)

declare i1 @printnum(i32)

declare i1 @printr(i32)

declare i1 @print(i32)

declare i1 @wait(i32)

declare i32 @usclock()

declare i32 @getPID()

define i1 @sleep(i32 %i_arg) {
entry:
  %i = alloca i32
  %var_tmp = load i32, i32* %i
  %mul_tmp = mul i32 %var_tmp, 1000
  %call_tmp = call i1 @wait(i32 %mul_tmp)
  ret i1 false
}

define i32 @main() {
entry:
  %call_tmp = call i1 @Start()
  %call_tmp1 = call i1 @gpioMode(i32 22, i32 0)
  %call_tmp2 = call i1 @gpioMode(i32 23, i32 0)
  %call_tmp3 = call i1 @gpioWrite(i32 22, i32 1)
  %call_tmp4 = call i1 @wait(i32 500000)
  %call_tmp5 = call i1 @gpioWrite(i32 23, i32 0)
  %call_tmp6 = call i1 @wait(i32 500000)
  %call_tmp7 = call i1 @gpioWrite(i32 22, i32 1)
  %call_tmp8 = call i1 @End()
  ret i32 0
}
