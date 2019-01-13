; ModuleID = 'cast.arow'
source_filename = "cast.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i32 @main() {
entry:
  %array = alloca [2 x double]*, i32 2
  %b = alloca i16
  store i16 2, i16* %b
  %var_tmp = load i16, i16* %b
  %0 = zext i16 %var_tmp to i32
  ret i32 %0
}
