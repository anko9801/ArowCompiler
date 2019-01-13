; ModuleID = 'uint.arow'
source_filename = "uint.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i32 @main() {
entry:
  %array = alloca [2 x double]*, i32 2
  %a = alloca i2
  store i2 1, i2* %a
  %var_tmp = load i2, i2* %a
  %0 = zext i2 %var_tmp to i32
  %call_tmp = call i1 @printnum(i32 %0)
  %var_tmp1 = load i2, i2* %a
  %1 = zext i2 %var_tmp1 to i32
  ret i32 %1
}
