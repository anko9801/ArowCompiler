; ModuleID = 'null.arow'
source_filename = "null.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i32 @none() {
entry:
  %array = alloca [2 x double]*, i32 2
  ret i32 0
}

define i32 @main() {
entry:
  %array = alloca [2 x double]*, i32 2
  %ab = alloca i32
  %call_tmp = call i32 @none()
  store i32 %call_tmp, i32* %ab
  %var_tmp = load i32, i32* %ab
  %ifcond = icmp eq i32 %var_tmp, 0
  br i1 %ifcond, label %then, label %else

then:                                             ; preds = %entry
  %call_tmp1 = call i1 @printnum(i32 1)
  %var_tmp2 = load i32, i32* %ab
  ret i32 %var_tmp2
  br label %ifcont

else:                                             ; preds = %entry
  %var_tmp3 = load i32, i32* %ab
  %call_tmp4 = call i1 @printnum(i32 %var_tmp3)
  %var_tmp5 = load i32, i32* %ab
  ret i32 %var_tmp5
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %var_tmp6 = load i32, i32* %ab
  ret i32 %var_tmp6
}
