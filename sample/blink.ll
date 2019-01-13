; ModuleID = 'blink.arow'
source_filename = "blink.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

declare i1 @confirm()

declare i1 @Close()

declare i1 @gpioMode(i32, i32)

declare i1 @gpioWrite(i32, i32)

declare i8 @gpioRead(i32)

declare i1 @gpioClear(i32)

define i32 @main() {
entry:
  %array = alloca [2 x double]*, i32 2
  %call_tmp = call i1 @printnum(i32 2)
  %call_tmp1 = call i1 @confirm()
  %call_tmp2 = call i1 @printnum(i32 2)
  %call_tmp3 = call i1 @gpioMode(i32 23, i32 1)
  %call_tmp4 = call i1 @printnum(i32 2)
  %i = alloca i32
  store i32 0, i32* %i
  %var_tmp = load i32, i32* %i
  %ifcond = icmp ne i32 %var_tmp, 5
  br i1 %ifcond, label %loop, label %afterloop

loop:                                             ; preds = %loop, %entry
  %var_tmp5 = load i32, i32* %i
  %call_tmp6 = call i1 @printnum(i32 %var_tmp5)
  %var_tmp7 = load i32, i32* %i
  %add_tmp = add i32 %var_tmp7, 1
  store i32 %add_tmp, i32* %i
  %call_tmp8 = call i1 @gpioWrite(i32 23, i32 1)
  %call_tmp9 = call i1 @wait(i32 1000000)
  %call_tmp10 = call i1 @gpioWrite(i32 23, i32 0)
  %call_tmp11 = call i1 @wait(i32 1000000)
  %var_tmp12 = load i32, i32* %i
  %ifcond13 = icmp ne i32 %var_tmp12, 5
  br i1 %ifcond13, label %loop, label %afterloop

afterloop:                                        ; preds = %loop, %entry
  %call_tmp14 = call i1 @gpioClear(i32 23)
  %call_tmp15 = call i1 @Close()
  ret i32 0
}
