; ModuleID = 'bit.arow'
source_filename = "bit.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i32 @main() {
entry:
  %array = alloca [2 x double]*, i32 2
  %a = alloca i32
  store i32 24, i32* %a
  %var_tmp = load i32, i32* %a
  %call_tmp = call i1 @printnum(i32 %var_tmp)
  ret i32 0
}
