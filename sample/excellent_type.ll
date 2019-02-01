; ModuleID = 'excellent_type.arow'
source_filename = "excellent_type.arow"

declare i1 @printnum(i32)

declare i1 @printr(i32)

declare i1 @print(i32)

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
  %test = alloca i32
  store i32 0, i32* %test
  %var_tmp = load i32, i32* %test
  %call_tmp = call i1 @printnum(i32 %var_tmp)
  ret i32 0
}
