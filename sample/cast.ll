; ModuleID = 'cast.arow'
source_filename = "cast.arow"

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
  %b = alloca i16
  store i16 2, i16* %b
  %var_tmp = load i16, i16* %b
  %0 = zext i16 %var_tmp to i32
  ret i32 %0
}
