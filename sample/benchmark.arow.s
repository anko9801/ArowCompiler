; ModuleID = 'benchmark.arow'
source_filename = "benchmark.arow"

declare i1 @printnum(i32)

declare i1 @sleep(i32)

declare i32 @usclock()

declare i1 @BlinkLED(i32)

declare i1 @GPIOsetup()

declare i1 @GPIOclear()

define i32 @main() {
entry:
  %array = alloca [2 x double]*, i32 2
  %i = alloca i32
  store i32 0, i32* %i
  %start = alloca i32
  %call_tmp = call i32 @usclock()
  store i32 %call_tmp, i32* %start
  %var_tmp = load i32, i32* %i
  %ifcond = icmp slt i32 %var_tmp, 1000000
  br label %loop

loop:                                             ; preds = %loop, %entry
  %var_tmp1 = load i32, i32* %i
  %add_tmp = add i32 %var_tmp1, 1
  store i32 %add_tmp, i32* %i
  %var_tmp2 = load i32, i32* %i
  %call_tmp3 = call i1 @printnum(i32 %var_tmp2)
  br i1 %ifcond, label %loop, label %afterloop

afterloop:                                        ; preds = %loop
  %call_tmp4 = call i32 @usclock()
  %var_tmp5 = load i32, i32* %start
  %sub_tmp = sub i32 %call_tmp4, %var_tmp5
  %call_tmp6 = call i1 @printnum(i32 %sub_tmp)
  %var_tmp7 = load i32, i32* %i
  ret i32 %var_tmp7
}
