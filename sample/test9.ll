; ModuleID = 'test9.arow'
source_filename = "test9.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i32 @main() {
entry:
  %array = alloca [2 x double]*, i32 2
  %cnt = alloca i32
  store i32 13, i32* %cnt
  %var_tmp = load i32, i32* %cnt
  %ifcond = icmp slt i32 %var_tmp, 20
  br i1 %ifcond, label %loop, label %afterloop

loop:                                             ; preds = %loop, %entry
  %var_tmp1 = load i32, i32* %cnt
  %add_tmp = add i32 %var_tmp1, 1
  store i32 %add_tmp, i32* %cnt
  %var_tmp2 = load i32, i32* %cnt
  %ifcond3 = icmp slt i32 %var_tmp2, 20
  br i1 %ifcond3, label %loop, label %afterloop

afterloop:                                        ; preds = %loop, %entry
  %var_tmp4 = load i32, i32* %cnt
  %call_tmp = call i1 @printnum(i32 %var_tmp4)
  %var_tmp5 = load i32, i32* %cnt
  ret i32 %var_tmp5
}
