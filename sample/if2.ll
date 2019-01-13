; ModuleID = 'if2.arow'
source_filename = "if2.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i32 @main() {
entry:
  %array = alloca [2 x double]*, i32 2
  %a = alloca i32
  store i32 2, i32* %a
  %var_tmp = load i32, i32* %a
  %ifcond = icmp sgt i32 %var_tmp, 0
  br i1 %ifcond, label %then, label %else5

then:                                             ; preds = %entry
  %var_tmp1 = load i32, i32* %a
  %ifcond2 = icmp slt i32 %var_tmp1, 10
  br i1 %ifcond2, label %then3, label %else

then3:                                            ; preds = %then
  %var_tmp4 = load i32, i32* %a
  %call_tmp = call i1 @printnum(i32 %var_tmp4)
  br label %ifcont

else:                                             ; preds = %then
  br label %ifcont

ifcont:                                           ; preds = %else, %then3
  br label %ifcont6

else5:                                            ; preds = %entry
  br label %ifcont6

ifcont6:                                          ; preds = %else5, %ifcont
  %var_tmp7 = load i32, i32* %a
  ret i32 %var_tmp7
}
