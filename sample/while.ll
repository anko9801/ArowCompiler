; ModuleID = 'while.arow'
source_filename = "while.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i32 @main() {
entry:
  %array = alloca [2 x double]*, i32 2
  %i = alloca i32
  store i32 0, i32* %i
  %var_tmp = load i32, i32* %i
  %ifcond = icmp slt i32 %var_tmp, 5
  br i1 %ifcond, label %loop, label %afterloop

loop:                                             ; preds = %loop, %entry
  %var_tmp1 = load i32, i32* %i
  %add_tmp = add i32 %var_tmp1, 1
  store i32 %add_tmp, i32* %i
  %var_tmp2 = load i32, i32* %i
  %call_tmp = call i1 @printnum(i32 %var_tmp2)
  %var_tmp3 = load i32, i32* %i
  %ifcond4 = icmp slt i32 %var_tmp3, 5
  br i1 %ifcond4, label %loop, label %afterloop

afterloop:                                        ; preds = %loop, %entry
  ret i32 0
}
