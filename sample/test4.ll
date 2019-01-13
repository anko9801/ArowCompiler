; ModuleID = 'test4.arow'
source_filename = "test4.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i32 @main() {
entry:
  %array = alloca [2 x double]*, i32 2
  %k = alloca i32
  store i32 23, i32* %k
  %var_tmp = load i32, i32* %k
  %call_tmp = call i1 @printnum(i32 %var_tmp)
  ret i32 0
}
