; ModuleID = 'test9.arow'
source_filename = "test9.arow"

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
  %cnt = alloca i32
  store i32 63, i32* %cnt
  %var_tmp = load i32, i32* %cnt
  %0 = trunc i32 %var_tmp to i4
  %1 = zext i4 %0 to i32
  %call_tmp = call i1 @printnum(i32 %1)
  ret i32 0
}
