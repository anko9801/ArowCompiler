; ModuleID = 'uint.arow'
source_filename = "uint.arow"

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

define i33 @main() {
entry:
  %a = alloca i3
  store i3 -3, i3* %a
  %var_tmp = load i3, i3* %a
  %0 = zext i3 %var_tmp to i32
  %call_tmp = call i1 @printnum(i32 %0)
  %var_tmp1 = load i3, i3* %a
  %1 = zext i3 %var_tmp1 to i33
  ret i33 %1
}
